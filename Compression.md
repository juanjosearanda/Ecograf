Compression Methods in Digital Image Processing

https://github.com/juanjosearanda/Ecograf/blob/main/Papers/Evaluaci%C3%B3n%20de%20Algoritmos%20para%20Compresi%C3%B3n.pdf

The sources discuss different compression methods used in digital image processing, emphasizing the need for efficient storage and transmission of images, especially with the increasing use of digital imaging technologies. The article primarily focuses on lossy compression, where some information is discarded to achieve higher compression ratios.

Lossless vs. Lossy Compression
The sources explain that compression techniques can be classified into two main categories: lossless and lossy.

Lossless compression methods ensure that the decompressed image is identical to the original. These methods are suitable for applications where preserving every detail is crucial, such as medical imaging or archival purposes. [1]
Lossy compression methods, on the other hand, achieve higher compression ratios by discarding some image information deemed less important. These methods exploit the limitations of the human visual system, removing data that is less likely to be perceived. [1]
Specific Compression Methods
The sources specifically evaluate the following compression algorithms:

Run Length Encoding: This method replaces sequences of identical pixels with a count of the repeated pixel value and the value itself. This technique is particularly effective for images with large areas of uniform color. [1]
Huffman Coding: This is a variable-length coding technique that assigns shorter codes to more frequent pixel values, resulting in reduced file size. [1]
Lempel-Ziv-Welch (LZW): This dictionary-based compression algorithm identifies recurring patterns in the data and replaces them with shorter codes. [1]
Transform Coding Methods: These methods, such as the Karhunen-Loeve Transform, involve transforming the image data into a different domain, where it can be represented more efficiently. [1]
Vector Quantization: This technique groups similar pixel values into vectors and represents them using a codebook. [1]
Evaluation Criteria
The sources compare these algorithms based on several criteria:

Compression Ratio: This measures the reduction in file size achieved by the compression method. A higher compression ratio indicates better compression. [2]
Processing Time: This evaluates the time taken to compress and decompress the image. [2]
Image Quality: This assesses the subjective or objective difference between the original and decompressed images. [1]
Experimental Results
The authors tested these algorithms on various image formats, including grayscale, color, and eye-catcher images. The results, presented in Table 1, indicate that: [2]

Huffman coding achieved the highest compression ratio (80), closely followed by Run Length Encoding (70).
Transform coding methods and vector quantization offered moderate compression ratios (55 and 65, respectively).
LZW yielded the lowest compression ratio (50) among the tested algorithms.
The study also found that the processing time varied significantly between algorithms. [2] However, the specific processing times for each algorithm are not provided in the table.

Future Considerations
The authors highlight that ongoing research in image compression is focusing on developing more advanced techniques, particularly for compressing moving images (video sequences). They suggest that combining multiple compression methods, such as fractal coding and Huffman coding, could lead to even more efficient compression strategies in the future. [3]

The sources also acknowledge the growing influence of hardware advancements on compression algorithms, emphasizing the need to develop methods that can leverage the increasing processing power of modern computer systems. [3]

