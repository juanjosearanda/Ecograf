/*
 * Segmentación de Secuencias de Imágenes Ecocardiográficas
 * Implementación basada en el paper de J. J. Aranda (ICID, 1996-97)
 * 
 * Compilación:
 * g++ -std=c++17 -O3 -Wall -Wextra segmentacion_kalman.cpp -o kalman_segmenter \
 *     `pkg-config --cflags --libs opencv4`
 * 
 * Uso:
 * ./kalman_segmenter                          # Usa video sintético
 * ./kalman_segmenter -fn=video.avi            # Procesa archivo
 * ./kalman_segmenter --file_name=video.avi    # Forma larga
 */

#include <opencv2/opencv.hpp>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <cmath>
#include <string>
#include <cstdlib>

// Constantes de seguridad
constexpr int MAX_WIDTH = 1920;
constexpr int MAX_HEIGHT = 1080;
constexpr int MIN_DIMENSION = 1;

class KalmanSegmenter {
private:
    // Parámetros del algoritmo (del paper)
    const float alpha = 0.01f;  // Ganancia baja para movimiento
    const float beta = 0.1f;    // Ganancia alta para fondo
    const float a12 = 0.25f;    // Dinámica de variación del fondo
    const float a22 = 0.25f;
    const float maxdif = 16.0f; // Umbral de detección

    // Estado del filtro
    cv::Mat B_est;   // Fondo estimado
    cv::Mat B_med;   // Componente auxiliar
    cv::Mat mask;    // Máscara binaria

    int height;
    int width;

    // Validación de dimensiones
    void validateDimensions(int h, int w) {
        if (w < MIN_DIMENSION || w > MAX_WIDTH || 
            h < MIN_DIMENSION || h > MAX_HEIGHT) {
            throw std::invalid_argument(
                "Dimensiones inválidas: " + std::to_string(w) + "x" + 
                std::to_string(h) + ". Rango permitido: [" + 
                std::to_string(MIN_DIMENSION) + "-" + std::to_string(MAX_WIDTH) + 
                "]x[" + std::to_string(MIN_DIMENSION) + "-" + 
                std::to_string(MAX_HEIGHT) + "]"
            );
        }
    }

public:
    KalmanSegmenter(int h, int w) : height(h), width(w) {
        validateDimensions(h, w);

        // Inicialización del estado (Paso 0 del paper)
        B_est = cv::Mat(height, width, CV_32F, cv::Scalar(127.5f));
        B_med = cv::Mat::zeros(height, width, CV_32F);
        mask = cv::Mat::ones(height, width, CV_32F);

        std::cout << "KalmanSegmenter inicializado: " << width << "x" << height << std::endl;
    }

    // Procesa un frame y retorna máscara y fondo estimado
    void processFrame(const cv::Mat& frame_gray, cv::Mat& out_mask, cv::Mat& out_background) {
        // Validación de entrada
        if (frame_gray.empty()) {
            throw std::invalid_argument("Frame vacío");
        }
        if (frame_gray.type() != CV_8U) {
            throw std::invalid_argument("Frame debe ser CV_8U (escala de grises)");
        }
        if (frame_gray.rows != height || frame_gray.cols != width) {
            throw std::invalid_argument("Dimensiones del frame no coinciden");
        }

        // Convertir a float32 para cálculos
        cv::Mat I_k;
        frame_gray.convertTo(I_k, CV_32F);

        // --- Paso 1: Actualización de la medición ---
        
        // Matriz de ganancia G adaptativa
        cv::Mat G = alpha * mask + beta * (1.0f - mask);

        // Innovación: diferencia entre imagen actual y fondo estimado
        cv::Mat innovation = I_k - B_est;

        // Corrección del estado
        cv::Mat B_est_corr = B_est + G.mul(innovation);
        cv::Mat B_med_corr = B_med + G.mul(innovation);

        // --- Paso 2: Predicción del fondo (para k+1) ---
        
        B_est = B_est_corr + a12 * B_med_corr;
        B_med = a22 * B_med_corr;

        // Asegurar rango válido [0, 255]
        cv::threshold(B_est, B_est, 255.0, 255.0, cv::THRESH_TRUNC);
        cv::max(B_est, 0.0, B_est);

        // --- Paso 3: Detección (Generación de máscara) ---
        
        // Diferencia absoluta
        cv::Mat D_k;
        cv::absdiff(I_k, B_est, D_k);

        // Umbralización: 1.0 si D_k >= maxdif, 0.0 si no
        cv::Mat new_mask;
        cv::threshold(D_k, new_mask, maxdif, 1.0, cv::THRESH_BINARY);

        // Actualizar máscara para siguiente iteración
        mask = new_mask;

        // Convertir a uint8 para visualización
        mask.convertTo(out_mask, CV_8U, 255.0);
        B_est.convertTo(out_background, CV_8U);
    }
};

// Generador de frames sintéticos (equivalente a la función Python)
class SyntheticHeartGenerator {
private:
    int width;
    int height;
    int frame_count;
    cv::RNG rng;

public:
    SyntheticHeartGenerator(int w = 320, int h = 240) 
        : width(w), height(h), frame_count(0), rng(cv::getTickCount()) {
        std::cout << "Generador sintético inicializado: " << w << "x" << h << std::endl;
    }

    cv::Mat generateFrame() {
        cv::Mat img = cv::Mat::zeros(height, width, CV_8U);

        // Ruido de fondo (speckle noise típico de ultrasonido)
        cv::Mat noise(height, width, CV_8U);
        rng.fill(noise, cv::RNG::NORMAL, 50, 15);
        cv::add(img, noise, img);

        // Simular corazón (círculo que se contrae y expande)
        cv::Point center(width / 2, height / 2);
        double t = frame_count * 0.2;
        int radius = static_cast<int>(60 + 20 * std::sin(t));
        int thickness = 15;

        // Dibujar "miocardio" (tejido brillante)
        cv::circle(img, center, radius, cv::Scalar(200), thickness);

        // Añadir ruido extra al tejido
        cv::Mat tissue_noise(height, width, CV_8U);
        rng.fill(tissue_noise, cv::RNG::NORMAL, 0, 20);
        
        cv::Mat mask_circle = cv::Mat::zeros(height, width, CV_8U);
        cv::circle(mask_circle, center, radius, cv::Scalar(255), thickness);
        
        tissue_noise.copyTo(img, mask_circle);

        frame_count++;
        return img;
    }

    void reset() {
        frame_count = 0;
    }
};

// Parser de argumentos de línea de comandos
class ArgumentParser {
private:
    std::string filename;
    bool use_synthetic;

public:
    ArgumentParser() : use_synthetic(true) {}

    bool parse(int argc, char* argv[]) {
        if (argc == 1) {
            // Sin argumentos, usar sintético
            use_synthetic = true;
            return true;
        }

        for (int i = 1; i < argc; ++i) {
            std::string arg(argv[i]);
            
            // Buscar -fn= o --file_name=
            if (arg.rfind("-fn=", 0) == 0) {
                filename = arg.substr(4);
                use_synthetic = false;
            } else if (arg.rfind("--file_name=", 0) == 0) {
                filename = arg.substr(12);
                use_synthetic = false;
            } else if (arg == "-h" || arg == "--help") {
                printHelp();
                return false;
            } else {
                std::cerr << "Argumento desconocido: " << arg << std::endl;
                printHelp();
                return false;
            }
        }

        if (!use_synthetic && filename.empty()) {
            std::cerr << "Error: nombre de archivo vacío" << std::endl;
            return false;
        }

        return true;
    }

    bool useSynthetic() const { return use_synthetic; }
    std::string getFilename() const { return filename; }

    void printHelp() const {
        std::cout << "\nUso: kalman_segmenter [opciones]\n\n"
                  << "Opciones:\n"
                  << "  -fn=ARCHIVO          Procesar archivo de video especificado\n"
                  << "  --file_name=ARCHIVO  Forma larga de -fn\n"
                  << "  -h, --help           Mostrar esta ayuda\n\n"
                  << "Si no se proporciona archivo, se usa video sintético.\n"
                  << std::endl;
    }
};

// Función principal
int main(int argc, char* argv[]) {
    try {
        std::cout << "=== Segmentación Ecocardiográfica con Filtro de Kalman ===" << std::endl;
        std::cout << "Basado en el paper de J. J. Aranda (ICID, 1996-97)\n" << std::endl;

        // Parsear argumentos
        ArgumentParser parser;
        if (!parser.parse(argc, argv)) {
            return 1;
        }

        // Inicializar fuente de video
        std::unique_ptr<cv::VideoCapture> cap;
        std::unique_ptr<SyntheticHeartGenerator> synth_gen;
        
        int width, height;
        bool use_synthetic = parser.useSynthetic();

        if (use_synthetic) {
            std::cout << "Modo: VIDEO SINTÉTICO" << std::endl;
            width = 320;
            height = 240;
            synth_gen = std::make_unique<SyntheticHeartGenerator>(width, height);
        } else {
            std::cout << "Modo: ARCHIVO DE VIDEO" << std::endl;
            std::cout << "Abriendo: " << parser.getFilename() << std::endl;
            
            cap = std::make_unique<cv::VideoCapture>(parser.getFilename());
            
            if (!cap->isOpened()) {
                throw std::runtime_error("No se puede abrir el archivo: " + parser.getFilename());
            }

            width = static_cast<int>(cap->get(cv::CAP_PROP_FRAME_WIDTH));
            height = static_cast<int>(cap->get(cv::CAP_PROP_FRAME_HEIGHT));

            if (width <= 0 || height <= 0) {
                throw std::runtime_error("Dimensiones inválidas del video");
            }

            double fps = cap->get(cv::CAP_PROP_FPS);
            int frame_count = static_cast<int>(cap->get(cv::CAP_PROP_FRAME_COUNT));
            std::cout << "Resolución: " << width << "x" << height << std::endl;
            std::cout << "FPS: " << fps << " | Frames: " << frame_count << std::endl;
        }

        // Inicializar segmentador
        KalmanSegmenter segmenter(height, width);

        std::cout << "\nPresiona 'q' para salir\n" << std::endl;

        // Loop principal
        int frame_num = 0;
        while (true) {
            cv::Mat frame_gray;

            // Obtener frame
            if (use_synthetic) {
                frame_gray = synth_gen->generateFrame();
            } else {
                cv::Mat frame;
                if (!cap->read(frame)) {
                    std::cout << "Fin del video o error de lectura" << std::endl;
                    break;
                }
                cv::cvtColor(frame, frame_gray, cv::COLOR_BGR2GRAY);
            }

            // Procesar con Kalman
            cv::Mat mask, background_est;
            segmenter.processFrame(frame_gray, mask, background_est);

            // Visualización
            // Crear overlay verde para la máscara
            cv::Mat mask_color;
            cv::cvtColor(mask, mask_color, cv::COLOR_GRAY2BGR);
            mask_color.setTo(cv::Scalar(0, 255, 0), mask); // Solo canal verde

            cv::Mat frame_color;
            cv::cvtColor(frame_gray, frame_color, cv::COLOR_GRAY2BGR);

            // Superponer imagen + máscara semitransparente
            cv::Mat overlay;
            cv::addWeighted(frame_color, 0.7, mask_color, 0.3, 0, overlay);

            // Mostrar ventanas
            cv::imshow("1. Entrada (ECO)", frame_gray);
            cv::imshow("2. Fondo Estimado (Kalman)", background_est);
            cv::imshow("3. Segmentacion Detectada", overlay);

            // Control de velocidad y salida
            char key = static_cast<char>(cv::waitKey(30));
            if (key == 'q' || key == 'Q' || key == 27) { // 'q' o ESC
                std::cout << "Saliendo..." << std::endl;
                break;
            }

            frame_num++;
            if (frame_num % 100 == 0) {
                std::cout << "Frames procesados: " << frame_num << std::endl;
            }
        }

        cv::destroyAllWindows();
        std::cout << "\nTotal de frames procesados: " << frame_num << std::endl;
        std::cout << "Finalizado exitosamente." << std::endl;

        return 0;

    } catch (const std::exception& e) {
        std::cerr << "\n[ERROR] " << e.what() << std::endl;
        cv::destroyAllWindows();
        return 1;
    } catch (...) {
        std::cerr << "\n[ERROR] Excepción desconocida" << std::endl;
        cv::destroyAllWindows();
        return 1;
    }
}