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

// *** Things to be implemented yet ***
// 1. 	If user signals an exit, the program must be able to safely stop the functionality, irrespective of it's current state and
//		exit the operation.
//		It must be able to close the file pointer, remove the binaries from the disk path and exit the program with a message

// Includes
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/statvfs.h>

// Defines
#define ARRAY_SIZE(array) (sizeof(array) / sizeof(array[0]))

// Function declarations
void test_zeros_write_speed(char *disk_path, uint64_t disk_size_free);
void test_random_write_speed(char *disk_path, uint64_t disk_size_free);
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
		fprintf(stderr, "Provided disk path does not exist\n");
		exit(1);
	}
	
	// Calculate the size of free space available on the disk
	uint64_t disk_size_free = get_size_free_space(disk_path);
	printf("Free space : %lu \n", disk_size_free);

	// Test the write speed of the disk
	test_zeros_write_speed(disk_path, disk_size_free);
}

// Test the write speed of the disk
void test_zeros_write_speed(char *disk_path, uint64_t disk_size_free)
{
	// Read the disk_path and copy them to the variables holding the path to the disk files
	char file_zeros_disk_path[1000];
	strcpy(file_zeros_disk_path, disk_path);
	
	// Modify the path to hold the path to the binary file in the disk
	int len = strlen(disk_path);
	if((disk_path[len-1]) == '/') strcat(file_zeros_disk_path, "test_write_zeros.bin");
	else strcat(file_zeros_disk_path, "/test_write_zeros.bin");

	// Open the disk_path in write mode
	FILE *file_zeros_disk_fp = fopen(file_zeros_disk_path, "w");
	if(file_zeros_disk_fp == NULL)
	{
		fprintf(stderr, "Error opening file on disk\n");
		exit(1);
	}

	// Check the size of the disk. If the size of disk is > 5GB then write a 5 GB data (random) to the file.
	// Else write the (disk_size_free - 500 MB) of data (random) to the file.
	// 5GB = 5368709120 Bytes, the 2KB (2048 Bytes) are just for some headspace so that we dont accidentally fill the entire disk.
	uint64_t allowed_f_size = 0;
	if(disk_size_free > (5368709120 + 2048)) allowed_f_size = 5368709120;
	else allowed_f_size = disk_size_free - 2048;

	// Print thw allowed file size to be written
	printf("Allowed file size : %lu\n", allowed_f_size);

	// Open the dev/zero file (we need this file to write all zeros to the disk in order to test the speed of the disk).
	FILE *zero_fp = fopen("/dev/zero", "r");
	if(zero_fp == NULL)
	{
		fprintf(stderr, "Error opening /dev/zero\n");
		exit(1);
	}

	// ----------------------------------------------------------------------
	// Calculate zeros write speed
	printf("\nCalulating zeros write speed ...\n\n");	

	// Initialize the clock for calculating time
	time_t start, end;
	uint64_t time_elapsed = 0.0;
	uint64_t written_f_size;
	
	// Keep reading the /dev/zero file until we reach the maximum allowed file size, and keep writing to the disk drive
	char buffer_zeros[1024];

	// Start the clock and start writing to the file
	start = time(NULL);
	while(fread(buffer_zeros, sizeof(*buffer_zeros), ARRAY_SIZE(buffer_zeros), zero_fp))
	{
		fwrite(buffer_zeros, sizeof(*buffer_zeros), ARRAY_SIZE(buffer_zeros), file_zeros_disk_fp);

		// Get the current postion of the file pointer to get the size of the file written
		// If the size of the zeros file has passed 5 GB stop the operation, break the loop
		
		// Remove this -- for debug only
		allowed_f_size = 2147483648;

		written_f_size = ftell(file_zeros_disk_fp);
		if(written_f_size >= allowed_f_size) break;
	}

	// End the clock
	end = time(NULL);

	// ----------------------------------------------------------------------

	// Close the file pointer and remove the file
	fclose(zero_fp);
	fclose(file_zeros_disk_fp);
	remove(file_zeros_disk_path);

	// Calculate the print the results
	time_elapsed = difftime(end, start);
	uint64_t speed_in_bytes = written_f_size / time_elapsed;
	double speed_in_mbytes = speed_in_bytes / 1048576;
	printf("Size of data (/dev/zero) written : %lu Bytes\n", written_f_size);
	printf("Time elapsed : %lu seconds\n", time_elapsed);
	printf("Data write speed : %lu Bps or %0.1f MBps\n", speed_in_bytes, speed_in_mbytes);
}

// Test the speed of writing random data on the disk
void test_random_write_speed(char *disk_path, uint64_t disk_size_free)
{
	// Read the disk_path and copy them to the variables holding the path to the disk files
	char file_random_disk_path[1000];
	strcpy(file_random_disk_path, disk_path);

	// Modify the path to hold the path to the binary file in the disk
	int len = strlen(disk_path);
	if((disk_path[len-1]) == '/') strcat(file_random_disk_path, "test_write_random.bin");
	else strcat(file_random_disk_path, "/test_write_random.bin");
	
	// Open the disk_path to test_write_random.bin in write mode
	FILE *file_random_disk_fp = fopen("file_random_disk_path", "w");
	if(file_random_disk_fp == NULL)
	{
		fprintf(stderr, "Error opening file on disk\n");
		exit(1);
	}

	// Check the size of the disk. If the size of disk is > 5GB then write a 5 GB data (random) to the file.
	// Else write the (disk_size - 500 MB) of data (random) to the file.
	// 5GB = 5368709120 Bytes
	uint64_t allowed_f_size = 0;
	if(disk_size_free > 5368709120) allowed_f_size = 5368709120;
	else allowed_f_size = disk_size_free - 524288000;

	// Print the allowed file size that can we written
	printf("Allowed file size : %lu\n", allowed_f_size);

	// Open the dev/random file (we need this file to write all random values to the disk in order to test the speed of the disk).
	FILE *random_fp = fopen("/dev/random", "r");
	if(random_fp == NULL)
	{
		fprintf(stderr, "Error opening /dev/random \n");
		exit(1);
	}

	// ----------------------------------------------------------------------
	// Calculate random data write speedrandom
	printf("\nCalculating random write speed ...\n\n");
	
	// Initialize the clock for calculating time
	clock_t start, end;
	uint64_t time_elapsed = 0.0;
	uint64_t written_f_size;

	// Keep reading from /dev/random and write the contents to the file on the disk
	char buffer_random[1024];

	// Start the clock and start writing to the file
	start = clock();
	while(fread(buffer_random, sizeof(*buffer_random), ARRAY_SIZE(buffer_random), random_fp))
	{
		// Write the contents of buffer to the file on disk
		fwrite(buffer_random, sizeof(*buffer_random), ARRAY_SIZE(buffer_random), file_random_disk_fp);

		// Get the current postion of the file pointer to get the size of the file written
		// If the size of the zeros file has passed 5 GB stop the operation, break the loop
		written_f_size = ftell(file_random_disk_fp);
		if(written_f_size > allowed_f_size) break;
	}
	end = clock();

	// ----------------------------------------------------------------------

	// Close all the file pointers and remove the created files
	fclose(random_fp);
	fclose(file_random_disk_fp);
	remove(file_random_disk_path);

	// Calculate the print the results
	time_elapsed = difftime(end, start);
	uint64_t speed_in_bytes = written_f_size / time_elapsed;
	double speed_in_mbytes = speed_in_bytes / 1048576;
	printf("Size of data (/dev/random) written : %lu Bytes\n", written_f_size);
	printf("Time elapsed : %lu seconds\n", time_elapsed);
	printf("Data write speed : %lu Bps or %0.1f MBps\n", speed_in_bytes, speed_in_mbytes);
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