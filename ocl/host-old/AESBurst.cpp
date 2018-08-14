#include "BruteForcer.h"
#include <getopt.h>

void usage(const char *name) {
	cout << "Usage: " << name << " [OPTIONS] <keylist> <sample_file>" << endl;
	cout << "\tOptions:" << endl;
	cout << "\t\t-c, --crib <crib text>" << endl;
	cout << "\t\t-e, --encoding <hex or b64>" << endl;
	cout << "\t\t-i, --initialization_vector <vector>" << endl;
	cout << "\t\t-m, --mode <ECB, CBC, CTR>" << endl;
}
	

int main(int argc, char **argv) {

	//handle arguments
	//TODO: fix getopt
	if (argc < 3) {
		usage(argv[0]);
		exit(EXIT_FAILURE);
	}

	char *keylist_filename = argv[argc - 2];
	char *samples_filename = argv[argc - 1];

	const char *optstring = "c:e:i:m:";
	const struct option long_options[] = {
		{"initialization_vector", 	0, 	0, 	'i'},
		{"crib", 					0, 	0, 	'c'},
		{"encoding", 				0, 	0, 	'e'},
		{"mode", 					0, 	0, 	'm'},
		{0, 						0,	0, 	0}
	};

	int c;
	int option_index = 0;



	//init the configuration struct
	//this struct is passed to the library to specify how to brute force
	BruteBuilder *config = new BruteBuilder();
	config->crib = "";
	config->is_b64 = false;
	config->iv = "";
	config->mode = ALL;
	while ((c = getopt_long(argc, argv, optstring, long_options, &option_index)) != EOF) {
		/* if (!optarg) { */
		/* 	cout << "Flag must be provided with an argument." << endl; */
		/* 	exit(EXIT_FAILURE); */
		/* } */
		string optargstr = string(optarg);
		switch (c) {
			case 'c':
				/* cout << "Setting crib to be " << optarg << endl; */
				config->crib = optargstr;
				break;

			case 'e':
				if (optargstr == "hex") {
					/* cout << "Choosing hex encoding." << endl; */
					config->is_b64 = false;
				} else if (optargstr == "b64") {
					/* cout << "Choosing base 64 encoding." << endl; */
					config->is_b64 = true;
				} else {
					/* cout << "Could not understand encoding. Please use 'hex' or 'b64'." */
						/* << endl << "Defaulting to 'hex' encoding." << endl; */
				}
				break;

			case 'i':
				/* cout << "Setting Initialization Vector to be " << optargstr << "." << endl; */
				config->iv = optargstr; //NOTE: this does not care about the null byte because IVs are always 16 bytes in AES
				break;

			case 'm':
				/* cout << "Setting mode to be " << optargstr << "." << endl; */
				Mode conf_mode = ALL;
				if (optargstr == "ECB") {
					conf_mode = ECB;
				} else if (optargstr == "CBC") {
					conf_mode = CBC;
				} else if (optargstr == "CTR") {
					conf_mode = CTR;
				}
				config->mode = conf_mode;
				break;
		}
	}
	
	//open files
	ifstream handle_keylist(keylist_filename);
	if (!handle_keylist.is_open()) {
		cout << "Could not open keylist." << endl;
		exit(EXIT_FAILURE);
	}

	ifstream handle_samples(samples_filename);
	if (!handle_samples.is_open()) {
		cout << "Could not open the samples file." << endl;
		exit(EXIT_FAILURE);
	}

	//initialize keys and samples on the heap
	vector<string> *keys = new vector<string>;
	vector<string> *samples = new vector<string>;

	//populate keys and samples
	string line;
	while (getline(handle_keylist, line)) {
		keys->push_back(line);
	}
	handle_keylist.close();

	while (getline(handle_samples, line)) {
		samples->push_back(line);
	}
	handle_samples.close();

	//actually try to brute force
	BruteForcer bf_aes(config);
	bf_aes.brute_force(keys, samples);

	//cleanup
	delete keys;
	delete samples;
	delete config;
}
