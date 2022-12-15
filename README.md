# c-qr-to-png
* This "library" bridges the qrcodegen library and the tinyPNGoutput library by nayuki. This is inspired/ported by the cpp implementation of user https://github.com/RaymiiOrg
 * This was written because our old codebase is not able to include external c code that uses streams. 
 * This program just uses pure c code to generate the QR code and write it as a png file.
 * It uses the C-versions of qrcodegen library by nayuki to generate the QR code and the tinyPNGoutput library by nayuki to write the png file.  
