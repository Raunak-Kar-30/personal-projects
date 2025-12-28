// Simple program for measuring the read and write speed of any disk.
// We do this by writing the /dev/zero (which is a file containing only binary zeros - or ASCII null character) file as well as the /dev/random (which only contains random gibberish)
// and by measuring the time elapsed, we can determine the write speed of the disk.
//
// Trying to learn some system programming by following a tutorial.
// It is not a one to one copy of the original program by Daniel Hirsch, so if you want the program code written by him, kindly follow the link given in
// the README.md file.
// I have modified the program according to my needs, as and when I have felt appropriate to do so.
//
// Author : Raunak Kar
// CREDITS : DANIEL HIRSCH from Youtube.

// Includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

// Defines
#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[i]))

// Function declarations
void test_write_speed(char *disk_path);
int disk_exists(char *disk_path);

// Main
int main(int argc, char *argv[])
{
	// Check if the correct number of arguments were provided.
	if(argc != 2)
	{
		fprintf(stderr, "Invalid arguments\nFormat : %s 'disk name'\n", argv[0]);
		return 1;
	}
	
	// Get the name of the disk drive path, and check if the given directory exists.
	char *disk_path = argv[1];
	if(!(disk_exists(disk_path)))
	{
		fprintf(stderr, "Provided disk path does not exists\n");
		end(1);
	}
	
	// Test the write speed of the disk
	test_write_speed(disk_path);
}

// Test the write speed of the disk
void test_write_speed(char *disk_path)
{
	char *file_zeros_disk_path = disk_path;
	char *file_random_disk_path = disk_path;

	int len = strlen(disk_path);
	if(disk_path[len-1] = '/') 
	{
		strcat(file_zeros_disk_path, "test_write_zeros.bin");
		strcat(file_random_disk_path, "test_write_random.bin");
	}
	else 
	{
		strcat(file_zeros_disk_path, "/test_write_zeros.bin");
		strcat(file_random_disk_path, "/test_write_random.bin");
	}

	// ----------------------------------------------------------------------
	// Calculate zeros write speed
	printf("Calulating zeros write speed ...\n");

	// Open the disk_path in write mode
	FILE *file_zero_disk_fp = fopen(file_zeros_disk_path, "w");
	if(file_zero_disk_fp == NULL)
	{
		fprintf(stderr, "Error opening file on disk\n");
		exit(1);
	}

	// Open the dev/zero file (we need this file to write all zeros to the disk in order to test the speed of the disk).
	FILE *zero_fp = fopen("/dev/zero", "r");
	if(zero_fp == NULL)
	{
		fprintf(stderr, "Error opening /dev/zero\n");
		exit(1);
	}

	// Keep reading the /dev/zero file until we reach the end of file, and keep writing to the disk drive
	char buffer[100];
	while(fread(buffer, sizeof(*buffer), ARRAY_SIZE(buffer), zero_fp))
	{
		fwrite(buffer, sizeof(*buffer), ARRAY_SIZE(buffer), file_zero_disk_fp);
	}

	// Close the file pointer and remove the file
	fclose(zero_fp);
	fclose(file_zeros_disk_fp);
	remove(file_zeros_disk_path);

	// Print out the result
	
	// ----------------------------------------------------------------------
	// Calculate random data write speed
	printf("Calculating random write speed ...\n");
	
	// Open the disk_path to test_write_random.bin in write mode
	FILE *file_random_disk_fp = fopen("file_random_disk_path", "w");
	if(file_random_disk_fp == NULL)
	{
		fprintf(stderr, "Error opening file on disk\n");
		exit(1);
	}

	// Open the dev/random file (we need this file to write all random values to the disk in order to test the speed of the disk).
	FILE *random_fp = fopen("/dev/random", "r");
	if(random_fp == NULL)
	{
		fprintf("Error opening /dev/random \n");
		exit(1);
	}

	// Keep reading from /dev/random and write the contents to the file on the disk
	char buffer[100];
	while(fread(buffer, sizeof(buffer), ARRAY_SIZE(buffer), random_fp))
	{
		fwrite(buffer, sizeof(buffer), ARRAY_SIZE(buffer), file_random_disk_fp);
	}

	// Close all the file pointers and remove the created files
	fclose(random_fp);
	fclose(file_random_disk_fp);
	remove(file_random_disk_path);

	// Print out the result
}

// Check if the disk exists
// Reference - https://codeforwin.org/c-programming/c-program-check-file-or-directory-exists-not
int disk_exists(char *disk_path)
{
	struct stat stats;

	stat(disk_path, &stats);
	if(S_ISDIR(stats.st_mode)) return 1;
	return 0;
}
