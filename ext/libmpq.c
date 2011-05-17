/**
 * ruby-libmpq
 * basic libmpq ruby binding
 *
 * FILE: libmpq.c
 * AUTHOR: Matt Perry
 * DATE: May 14 2011
 * DESCRIPTION:
 *  Use the libmpq C library to add a function
 *  to ruby which takes in a file name and reads
 *  the contents of that file from an MPQ archive
 */
#include <libmpq/mpq.h>
#include "ruby.h"

VALUE mpq_mod = Qnil;                                // MPQ Module
VALUE archive_class = Qnil;                          // Archive Class
void Init_mpq_read();                                // Initialization
void mpq_helper(VALUE self, mpq_archive_s** archive, unsigned int* file_number, VALUE file_name); // helper function
VALUE mpq_read_file(VALUE self, VALUE file_name);    // read_file function
VALUE mpq_file_size(VALUE self, VALUE file_name);    // file_size function

// Function that is called by Ruby to initialize the extention
void Init_mpq_read()
{
    // Define MPQ::Archive class and all functions
    mpq_mod = rb_define_module("MPQ");
    archive_class = rb_define_class_under(mpq_mod, "Archive", rb_cObject);
    rb_define_method(archive_class, "read_file", mpq_read_file, 1);
    rb_define_method(archive_class, "file_size", mpq_file_size, 1);
}

void mpq_helper(VALUE self, mpq_archive_s** archive, unsigned int* file_number, VALUE file_name)
{
    // Get the path to the archive from the ruby class
    VALUE archive_path = rb_funcall(self, rb_intern("path"), 0);

    // Open the archive, throw an exception if a failure occurs
    libmpq__init();
    int return_code = libmpq__archive_open(archive, RSTRING_PTR(archive_path), -1);
    if(return_code)
        rb_raise(rb_eException, "Error opening %s. Code: %d", archive_path, return_code);

    // Find the file in the archive and get its index, throw an exception if failure
    return_code = libmpq__file_number(*archive, RSTRING_PTR(file_name), file_number);
    if (return_code)
    {
        libmpq__archive_close(*archive);
        rb_raise(rb_eException, "No %s in %s", file_name, archive_path);
    }
}

// Function that becomes the extension
VALUE mpq_read_file(VALUE self, VALUE file_name)
{
    // Validate arguments
    Check_Type(file_name, T_STRING);

    mpq_archive_s* archive;         // Archive data struct
    unsigned int file_number;       // Index of this file
    off_t file_size;                // Size of this file
    char* file_contents;            // The contents of this file

    mpq_helper(self, &archive, &file_number, file_name); // Get file number

    // Find the size of the file, allocate memory, read the entire file
    libmpq__file_unpacked_size(archive, file_number, &file_size);
    file_contents = malloc(file_size);
    libmpq__file_read(archive, file_number, file_contents, file_size, NULL);

    // Copy the file into a Ruby readable buffer
    VALUE rb_buffer = rb_str_buf_new(file_size);
    rb_str_cat(rb_buffer, file_contents, file_size);

    // Cleanup and exit
    free(file_contents);
    libmpq__archive_close(archive);
    return rb_buffer;
}

VALUE mpq_file_size(VALUE self, VALUE file_name)
{
    // Validate arguments
    Check_Type(file_name, T_STRING);

    mpq_archive_s* archive;         // Archive data struct
    unsigned int file_number;       // Index of this file
    off_t file_size;                // Size of this file

    mpq_helper(self, &archive, &file_number, file_name); // Get file number

    // Find the size of the file
    libmpq__file_unpacked_size(archive, file_number, &file_size);

    // Cleanup and exit
    libmpq__archive_close(archive);
    return INT2FIX(file_size);
}
