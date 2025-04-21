import javax.imageio.ImageIO;
import java.awt.image.BufferedImage;
import java.io.File;
import java.io.IOException;

public class ReplaceNonTransparentPixels {
    // Set your custom RGB value here
    private static final int CUSTOM_R = 104;
    private static final int CUSTOM_G = 157;
    private static final int CUSTOM_B = 106;

    public static void main(String[] args) {

    	File dir = new File(args[0]);
    	for(File file : dir.listFiles()) {
    		fixColor(file,file);
    	}


    }

	private static void fixColor(File inputFile, File outputFile) {
        try {
            BufferedImage image = ImageIO.read(inputFile);

            for (int y = 0; y < image.getHeight(); y++) {
                for (int x = 0; x < image.getWidth(); x++) {
                    int argb = image.getRGB(x, y);
                    int alpha = (argb >> 24) & 0xff;

                    if (alpha != 0) {
                        // Replace pixel with custom RGB, preserve original alpha
                        int newARGB = (alpha << 24) | (CUSTOM_R << 16) | (CUSTOM_G << 8) | CUSTOM_B;
                        image.setRGB(x, y, newARGB);
                    }
                }
            }

            ImageIO.write(image, "png", outputFile);
            System.out.println("Image processed and saved to: " + outputFile.getAbsolutePath());

        } catch (IOException e) {
            System.err.println("Error processing image: " + e.getMessage());
        }		
	}
}