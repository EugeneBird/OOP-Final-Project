#include <iostream>
#include <fstream>

#include <opencv2/opencv.hpp>

using namespace std;
using namespace cv;


//�C�@��bit��Pos��and�A�H�P�_�O1��0
bool isBitSet(char ch, int pos) {
	// 7 6 5 4 3 2 1 0
	ch = ch >> pos;//Move several positions to the right
	if(ch & 1)//ch and 1=1?
		return true;
	return false;
}

int main(int argc, char** argv) {

	/*
	./encode image.png textfile.txt 
	argv[0] = ./encode
	argv[1] = image.png
	argv[2] = textfile.txt
	*/

	// Checks if proper number of arguments are passed
	if(argc != 3) {
		cout << "Arguments Error" << "\n";
		return 1;
	}

	// Stores original image
	Mat image = imread(argv[1]);
	if(image.empty()) {
		cout << "Image Error\n";
		return 1;
	}

	// Open file for text information
	ifstream file(argv[2]);
	if(!file.is_open()) {
		cout << "File Error\n";
		return 1;
	}


	char ch;
	file.get(ch);//�q�ɮק���r���A�H8bit��ascii code���
	// contains information about which bit of char to work on
	int bit_count = 0;
	// to check whether file has ended
	bool last_null_char = false;
	// to check if the whole message is encoded or not
	bool encoded = false;

	/*
	To hide text into images. We are taking one char (8 bits) and each of the 8 bits are stored
	in the Least Significant Bits (LSB) of the pixel values (Red,Green,Blue).
	We are manipulating bits in such way that changing LSB of the pixel values will not make a huge difference.
	The image will still look similiar to the naked eye.
	*/
	//cout<< image.rows<<"\n";
	//cout<< image.cols<<"\n";

	for(int row=0; row < image.rows; row++) {
		for(int col=0; col < image.cols; col++) {
			for(int color=0; color < 3; color++) {
				// stores the pixel details
				Vec3b pixel = image.at<Vec3b>(Point(row,col));//����
				//veb3b�]�t�T�ӦV�� ��ܨC�ӦV�ת���Ƶ��c���Ouchar
				
				
				if(isBitSet(ch,7-bit_count))//�p�Gbit��1����
					pixel.val[color] |= 1;//pixel.val[color] or 1(�j��]��1)
				else
					pixel.val[color] &= ~1;//�����쪬

				// update the image with the changed pixel values
				image.at<Vec3b>(Point(row,col)) = pixel;

				// increment bit_count to work on next bit()
				bit_count++;

				// �p�GŪ���r�ɪ��̫�@�Ӧr�� �h���X�T�Ӱj���X�Ϥ�
				if(last_null_char && bit_count == 8) {
					encoded  = true;
					goto OUT;
				}

				// if bit_count is 8 we pick the next char from the file and work on it
				if(bit_count == 8) {
					bit_count = 0;
					file.get(ch);

					// if EndOfFile(EOF) is encountered insert NULL char to the image
					if(file.eof()) {
						last_null_char = true;
						ch = '\0';
					}
				}

			}
		}
	}
	
	OUT:;


	// whole message was not encoded
	if(!encoded) {
		cout << "Message too big. Try with larger image.\n";
		return 2;
	}


	//output ��X�Ϥ��ɦW�s��"encode+����ɪ��W�r"
	string output("encode-");
	output =  output +argv[1];
    imwrite(output,image);
    return 0;
}

