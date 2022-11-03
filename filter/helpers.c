#include "helpers.h"
#include <math.h>

// Convert image to grayscale
void grayscale(int height, int width, RGBTRIPLE image[height][width])
{
    //Our input is basically image[h][w], an array of RGBTRIPLE's (struct of Blue, Green and Red byte).
    //For this filter we must average the red, green and blue values and set the result to the
    //three channels of the new pixel.
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            float x = (image[i][j].rgbtBlue + image[i][j].rgbtGreen + image[i][j].rgbtRed) / 3.0;
            x = (int) round(x);
            image[i][j].rgbtBlue = x;
            image[i][j].rgbtGreen = x;
            image[i][j].rgbtRed = x;
        }
    }
    return;
}

// Reflect image horizontally
void reflect(int height, int width, RGBTRIPLE image[height][width])
{
    for (int i = 0; i < height; i++) //For every row
    {
        if (width % 2 != 0) //If there's an uneven number of columns.
        {
            for (int j = 0; j < floor(width / 2); j++) //Repeat until the number in the middle, not included.
            {
                RGBTRIPLE temp = image[i][width - 1 - j]; //Pixel swapping, first to last.
                image[i][width - 1 - j] = image[i][j];
                image[i][j] = temp;
            }
        }
        else //If there's an even number of columns.
        {
            for (int j = 0; j < width / 2; j++) //Repeat until the number before the middle point.
            {
                RGBTRIPLE temp = image[i][width - 1 - j];
                image[i][width - 1 - j] = image[i][j];
                image[i][j] = temp;
            }
        }

    }
    return;
}

// Blur image
void blur(int height, int width, RGBTRIPLE image[height][width])
{
    //Blur Box: take the pixels around, average each color, write each color.
    //First a temp array for the output, because changing image would change subsequent calculations.
    RGBTRIPLE output[height][width];

    //Loop through every pixel:
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            //Look for a more elegant solution maybe **
            //Surrounding squares to compute:
            //Compute center square:
            int avgRed = 0, avgGreen = 0, avgBlue = 0;
            float meantotal = 0;

            avgRed += image[i][j].rgbtRed;
            avgGreen += image[i][j].rgbtGreen;
            avgBlue += image[i][j].rgbtBlue;
            meantotal++;

            if ((i - 1) >= 0) //If top row exists
            {
                //Compute top center.
                avgRed += image[i - 1][j].rgbtRed;
                avgGreen += image[i - 1][j].rgbtGreen;
                avgBlue += image[i - 1][j].rgbtBlue;
                meantotal++;

                if ((j - 1) >= 0) //If top left exists.
                {
                    //Compute top left.
                    avgRed += image[i - 1][j - 1].rgbtRed;
                    avgGreen += image[i - 1][j - 1].rgbtGreen;
                    avgBlue += image[i - 1][j - 1].rgbtBlue;
                    meantotal++;
                }

                if ((j + 1) <= (width - 1)) //If top right exists.
                {
                    //Compute top right.
                    avgRed += image[i - 1][j + 1].rgbtRed;
                    avgGreen += image[i - 1][j + 1].rgbtGreen;
                    avgBlue += image[i - 1][j + 1].rgbtBlue;
                    meantotal++;
                }
            }

            if ((j - 1) >=  0) //If left-center exists
            {
                //Compute left.
                avgRed += image[i][j - 1].rgbtRed;
                avgGreen += image[i][j - 1].rgbtGreen;
                avgBlue += image[i][j - 1].rgbtBlue;
                meantotal++;
            }
            if ((j + 1) <= (width - 1)) //If right-center exists.
            {
                //Compute right center.
                avgRed += image[i][j + 1].rgbtRed;
                avgGreen += image[i][j + 1].rgbtGreen;
                avgBlue += image[i][j + 1].rgbtBlue;
                meantotal++;
            }

            if ((i + 1) <= (height - 1)) //If bottom row exists:
            {
                //Compute bottom center.
                avgRed += image[i + 1][j].rgbtRed;
                avgGreen += image[i + 1][j].rgbtGreen;
                avgBlue += image[i + 1][j].rgbtBlue;
                meantotal++;

                if ((j - 1) >= 0) //If bottom left exists.
                {
                    //Compute bottom left.
                    avgRed += image[i + 1][j - 1].rgbtRed;
                    avgGreen += image[i + 1][j - 1].rgbtGreen;
                    avgBlue += image[i + 1][j - 1].rgbtBlue;
                    meantotal++;
                }

                if ((j + 1) <= (width - 1)) //If bottom right exists.
                {
                    //Compute bottom right.
                    avgRed += image[i + 1][j + 1].rgbtRed;
                    avgGreen += image[i + 1][j + 1].rgbtGreen;
                    avgBlue += image[i + 1][j + 1].rgbtBlue;
                    meantotal++;
                }
            }
            //Assign mean value to each channel for each pixel.
            output[i][j].rgbtRed = (int) round(avgRed / meantotal);
            output[i][j].rgbtGreen = (int) round(avgGreen / meantotal);
            output[i][j].rgbtBlue = (int) round(avgBlue / meantotal);
        }
    }

    //Copy the values to the image
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            image[i][j] = output[i][j];
        }
    }
    return;
}

// Detect edges
void edges(int height, int width, RGBTRIPLE image[height][width])
{
    //Edges filter: the value for each channel at output = sqr (Gx ^2 + Gy ^2)
    RGBTRIPLE output[height][width];

    //Sobel operator:
    int Gx[3][3] = {{-1, 0, 1}, {-2, 0, 2}, {-1, 0, 1}};
    int Gy[3][3] = {{-1, -2, -1}, {0, 0, 0}, {1, 2, 1}};

    //Loop through every pixel:
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {

            typedef struct
            {
                int red;
                int blue;
                int green;
            }
            matrix_channels;

            matrix_channels Gx_value[3][3], Gy_value[3][3];
            //Populate the results matrix:
            for (int x = 0; x < 3; x++)
            {
                for (int y = 0; y < 3; y++)
                {
                    Gx_value[x][y].red = 0;
                    Gx_value[x][y].green = 0;
                    Gx_value[x][y].blue = 0;
                    Gy_value[x][y].red = 0;
                    Gy_value[x][y].green = 0;
                    Gy_value[x][y].blue = 0;
                }
            }

            //Look for a more elegant solution maybe **
            //Surrounding squares to compute:
            //Center square doesn't compute.
            //If top row exists
            if ((i - 1) >= 0)
            {
                //Compute top center (only Gy).
                Gy_value[0][1].red = image[i - 1][j].rgbtRed * Gy[0][1];
                Gy_value[0][1].green = image[i - 1][j].rgbtGreen * Gy[0][1];
                Gy_value[0][1].blue = image[i - 1][j].rgbtBlue * Gy[0][1];

                if ((j - 1) >= 0) //If top left exists.
                {
                    //Compute top left.
                    Gx_value[0][0].red = image[i - 1][j - 1].rgbtRed * Gx[0][0];
                    Gx_value[0][0].green = image[i - 1][j - 1].rgbtGreen * Gx[0][0];
                    Gx_value[0][0].blue = image[i - 1][j - 1].rgbtBlue * Gx[0][0];

                    Gy_value[0][0].red = image[i - 1][j - 1].rgbtRed * Gy[0][0];
                    Gy_value[0][0].green = image[i - 1][j - 1].rgbtGreen * Gy[0][0];
                    Gy_value[0][0].blue = image[i - 1][j - 1].rgbtBlue * Gy[0][0];
                }

                if ((j + 1) <= (width - 1)) //If top right exists.
                {
                    //Compute top right.
                    Gx_value[0][2].red = image[i - 1][j + 1].rgbtRed * Gx[0][2];
                    Gx_value[0][2].green = image[i - 1][j + 1].rgbtGreen * Gx[0][2];
                    Gx_value[0][2].blue = image[i - 1][j + 1].rgbtBlue * Gx[0][2];

                    Gy_value[0][2].red = image[i - 1][j + 1].rgbtRed * Gy[0][2];
                    Gy_value[0][2].green = image[i - 1][j + 1].rgbtGreen * Gy[0][2];
                    Gy_value[0][2].blue = image[i - 1][j + 1].rgbtBlue * Gy[0][2];
                }
            }

            if ((j - 1) >=  0) //If left-center exists
            {
                //Compute left (only Gx).
                Gx_value[1][0].red = image[i][j - 1].rgbtRed * Gx[1][0];
                Gx_value[1][0].green = image[i][j - 1].rgbtGreen * Gx[1][0];
                Gx_value[1][0].blue = image[i][j - 1].rgbtBlue * Gx[1][0];
            }

            if ((j + 1) <= (width - 1)) //If right-center exists.
            {
                //Compute right center (only Gx).
                Gx_value[1][2].red = image[i][j + 1].rgbtRed * Gx[1][2];
                Gx_value[1][2].green = image[i][j + 1].rgbtGreen * Gx[1][2];
                Gx_value[1][2].blue = image[i][j + 1].rgbtBlue * Gx[1][2];
            }

            if ((i + 1) <= (height - 1)) //If bottom row exists:
            {
                //Compute bottom center (only Gy).
                Gy_value[2][1].red = image[i + 1][j].rgbtRed * Gy[2][1];
                Gy_value[2][1].green = image[i + 1][j].rgbtGreen * Gy[2][1];
                Gy_value[2][1].blue = image[i + 1][j].rgbtBlue * Gy[2][1];

                if ((j - 1) >= 0) //If bottom left exists.
                {
                    //Compute bottom left.
                    Gx_value[2][0].red = image[i + 1][j - 1].rgbtRed * Gx[2][0];
                    Gx_value[2][0].green = image[i + 1][j - 1].rgbtGreen * Gx[2][0];
                    Gx_value[2][0].blue = image[i + 1][j - 1].rgbtBlue * Gx[2][0];

                    Gy_value[2][0].red = image[i + 1][j - 1].rgbtRed * Gy[2][0];
                    Gy_value[2][0].green = image[i + 1][j - 1].rgbtGreen * Gy[2][0];
                    Gy_value[2][0].blue = image[i + 1][j - 1].rgbtBlue * Gy[2][0];
                }

                if ((j + 1) <= (width - 1)) //If bottom right exists.
                {
                    //Compute bottom right.
                    Gx_value[2][2].red = image[i + 1][j + 1].rgbtRed * Gx[2][2];
                    Gx_value[2][2].green = image[i + 1][j + 1].rgbtGreen * Gx[2][2];
                    Gx_value[2][2].blue = image[i + 1][j + 1].rgbtBlue * Gx[2][2];

                    Gy_value[2][2].red = image[i + 1][j + 1].rgbtRed * Gy[2][2];
                    Gy_value[2][2].green = image[i + 1][j + 1].rgbtGreen * Gy[2][2];
                    Gy_value[2][2].blue = image[i + 1][j + 1].rgbtBlue * Gy[2][2];
                }
            }
            //Get the total Gx and Gy value for each channel.
            typedef struct
            {
                int red;
                int green;
                int blue;
            }
            kernel_totals;

            kernel_totals Gx_totals, Gy_totals;
            //Reset the totals struct
            Gx_totals.red = 0;
            Gx_totals.green = 0;
            Gx_totals.blue = 0;
            Gy_totals.red = 0;
            Gy_totals.green = 0;
            Gy_totals.blue = 0;

            for (int x = 0; x < 3; x++)
            {
                for (int y = 0; y < 3; y++)
                {
                    Gx_totals.red += Gx_value[x][y].red;
                    Gx_totals.green += Gx_value[x][y].green;
                    Gx_totals.blue += Gx_value[x][y].blue;

                    Gy_totals.red += Gy_value[x][y].red;
                    Gy_totals.green += Gy_value[x][y].green;
                    Gy_totals.blue += Gy_value[x][y].blue;
                }
            }
            //Assign value for each channel:
            int channel_total_red = 0, channel_total_green = 0, channel_total_blue = 0;
            channel_total_red = round(sqrt((Gx_totals.red * Gx_totals.red) + (Gy_totals.red * Gy_totals.red)));
            channel_total_green = round(sqrt((Gx_totals.green * Gx_totals.green) + (Gy_totals.green * Gy_totals.green)));
            channel_total_blue = round(sqrt((Gx_totals.blue * Gx_totals.blue) + (Gy_totals.blue * Gy_totals.blue)));

            if (channel_total_red > 255)
            {
                output[i][j].rgbtRed = 255;
            }
            else
            {
                output[i][j].rgbtRed = channel_total_red;
            }

            if (channel_total_green > 255)
            {
                output[i][j].rgbtGreen = 255;
            }
            else
            {
                output[i][j].rgbtGreen = channel_total_green;
            }

            if (channel_total_blue > 255)
            {
                output[i][j].rgbtBlue = 255;
            }
            else
            {
                output[i][j].rgbtBlue = channel_total_blue;
            }
        }
    }
    for (int i = 0; i < height; i++)
    {
        for (int j = 0; j < width; j++)
        {
            image[i][j] = output[i][j];
        }
    }
    return;
}
