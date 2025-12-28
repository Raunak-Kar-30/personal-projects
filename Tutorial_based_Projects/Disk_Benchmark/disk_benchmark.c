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
#include <stdint.h>
#include <sys/stat.h>
#include <sys/statvfs.h>

// Defines
#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

// Function declarations
void test_zeros_write_speed(char *disk_path);
void test_random_write_speed(char *disk_path);
int disk_exists(char *disk_path);
uint64_t get_size_free_space(char *disk_path);

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
		exit(1);
	}
	
	// Calculate the size of free space available on the disk
	uint64_t size = get_size_free_space(disk_path);
	printf("Free space : %lu \n", size);

	// Test the write speed of the disk
	//test_zeros_write_speed(disk_path);
}

// Test the write speed of the disk
void test_zeros_write_speed(char *disk_path)
{
	// Read the disk_path and copy them to the variables holding the path to the disk files
	char file_zeros_disk_path[1000];
	strcpy(file_zeros_disk_path, disk_path);
	
	// Modify the path to hold the path to the binary file in the disk
	int len = strlen(disk_path);
	if((disk_path[len-1]) == '/') strcat(file_zeros_disk_path, "test_write_zeros.bin");
	else strcat(file_zeros_disk_path, "/test_write_zeros.bin");

	// Debugging
	printf("Zeros disk path : %s\n", file_zeros_disk_path);

	// ----------------------------------------------------------------------
	// Calculate zeros write speed
	printf("Calulating zeros write speed ...\n");

	// Open the disk_path in write mode
	FILE *file_zeros_disk_fp = fopen(file_zeros_disk_path, "w");
	if(file_zeros_disk_fp == NULL)
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
	char buffer_zeros[100];
	while(fread(buffer_zeros, sizeof(*buffer_zeros), ARRAY_SIZE(buffer_zeros), zero_fp))
	{
		fwrite(buffer_zeros, sizeof(*buffer_zeros), ARRAY_SIZE(buffer_zeros), file_zeros_disk_fp);
	}

	// Close the file pointer and remove the file
	fclose(zero_fp);
	fclose(file_zeros_disk_fp);
	remove(file_zeros_disk_path);

	// Print out the result
}

// Test the speed of writing random data on the disk
void test_random_write_speed(char *disk_path)
{
	// Read the disk_path and copy them to the variables holding the path to the disk files
	char file_random_disk_path[1000];
	strcpy(file_random_disk_path, disk_path);

	// Modify the path to hold the path to the binary file in the disk
	int len = strlen(disk_path);
	if((disk_path[len-1]) == '/') strcat(file_random_disk_path, "test_write_random.bin");
	else strcat(file_random_disk_path, "/test_write_random.bin");

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
		fprintf(stderr, "Error opening /dev/random \n");
		exit(1);
	}

	// Keep reading from /dev/random and write the contents to the file on the disk
	char buffer_random[100];
	while(fread(buffer_random, sizeof(*buffer_random), ARRAY_SIZE(buffer_random), random_fp))
	{
		fwrite(buffer_random, sizeof(*buffer_random), ARRAY_SIZE(buffer_random), file_random_disk_fp);
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

// Calculates the size of the free space in the disk
uint64_t get_size_free_space(char *disk_path)
{
	// Create a dummy file (needed for statfs to calculate disk size)
	char file_dummy_disk_path[1000];
	strcpy(file_dummy_disk_path, disk_path);
	
	int len = strlen(file_dummy_disk_path);
	if(file_dummy_disk_path[len-1] == '/') strcat(file_dummy_disk_path, "dummy.txt");
	else strcat(file_dummy_disk_path, "/dummy.txt");

	printf("Dummy file : %s\n", file_dummy_disk_path);

	FILE *dummy_fp = fopen(file_dummy_disk_path, "w");
	if(dummy_fp == NULL)
	{
		fprintf(stderr, "Cannot open dummy file on disk for calculating free space\n");
		exit(1);
	}
	fclose(dummy_fp);

	// Create the struct for statvfs - this holds all the information about the disk
	struct statvfs stats;
	statvfs(file_dummy_disk_path, &stats);

	// Remove the dummy file
	remove(file_dummy_disk_path);

	// Return the free space on the disk
	return (stats.f_bfree * stats.f_frsize);
}