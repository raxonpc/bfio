/*
 * Library file type test program
 *
 * Copyright (C) 2009-2018, Joachim Metz <joachim.metz@gmail.com>
 *
 * Refer to AUTHORS for acknowledgements.
 *
 * This software is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This software is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this software.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <common.h>
#include <file_stream.h>
#include <narrow_string.h>
#include <system_string.h>
#include <types.h>
#include <wide_string.h>

#if defined( HAVE_STDLIB_H ) || defined( WINAPI )
#include <stdlib.h>
#endif

#include "bfio_test_functions.h"
#include "bfio_test_getopt.h"
#include "bfio_test_libbfio.h"
#include "bfio_test_libcerror.h"
#include "bfio_test_libclocale.h"
#include "bfio_test_libuna.h"
#include "bfio_test_macros.h"
#include "bfio_test_memory.h"

#include "../libbfio/libbfio_file_io_handle.h"

#if defined( HAVE_WIDE_SYSTEM_CHARACTER ) && SIZEOF_WCHAR_T != 2 && SIZEOF_WCHAR_T != 4
#error Unsupported size of wchar_t
#endif

/* Define to make bfio_test_file generate verbose output
#define BFIO_TEST_FILE_VERBOSE
 */

#if defined( __GNUC__ ) && !defined( LIBBFIO_DLL_IMPORT )

/* Tests the libbfio_file_io_handle_initialize function
 * Returns 1 if successful or 0 if not
 */
int bfio_test_file_io_handle_initialize(
     void )
{
	libbfio_file_io_handle_t *file_io_handle = NULL;
	libcerror_error_t *error                 = NULL;
	int result                               = 0;

	/* Test regular cases
	 */
	result = libbfio_file_io_handle_initialize(
	          &file_io_handle,
	          &error );

	BFIO_TEST_ASSERT_EQUAL_INT(
	 "result",
	 result,
	 1 );

	BFIO_TEST_ASSERT_IS_NOT_NULL(
	 "file_io_handle",
	 file_io_handle );

	BFIO_TEST_ASSERT_IS_NULL(
	 "error",
	 error );

	result = libbfio_file_io_handle_free(
	          &file_io_handle,
	          &error );

	BFIO_TEST_ASSERT_EQUAL_INT(
	 "result",
	 result,
	 1 );

	BFIO_TEST_ASSERT_IS_NULL(
	 "file_io_handle",
	 file_io_handle );

	BFIO_TEST_ASSERT_IS_NULL(
	 "error",
	 error );

	/* Test error cases
	 */
	result = libbfio_file_io_handle_initialize(
	          NULL,
	          &error );

	BFIO_TEST_ASSERT_EQUAL_INT(
	 "result",
	 result,
	 -1 );

	BFIO_TEST_ASSERT_IS_NOT_NULL(
	 "error",
	 error );

	libcerror_error_free(
	 &error );

	file_io_handle = (libbfio_file_io_handle_t *) 0x12345678UL;

	result = libbfio_file_io_handle_initialize(
	          &file_io_handle,
	          &error );

	file_io_handle = NULL;

	BFIO_TEST_ASSERT_EQUAL_INT(
	 "result",
	 result,
	 -1 );

	BFIO_TEST_ASSERT_IS_NOT_NULL(
	 "error",
	 error );

	libcerror_error_free(
	 &error );

#if defined( HAVE_BFIO_TEST_MEMORY )

	/* Test libbfio_file_io_handle_initialize with malloc failing
	 */
	bfio_test_malloc_attempts_before_fail = 0;

	result = libbfio_file_io_handle_initialize(
	          &file_io_handle,
	          &error );

	if( bfio_test_malloc_attempts_before_fail != -1 )
	{
		bfio_test_malloc_attempts_before_fail = -1;

		if( file_io_handle != NULL )
		{
			libbfio_file_io_handle_free(
			 &file_io_handle,
			 NULL );
		}
	}
	else
	{
		BFIO_TEST_ASSERT_EQUAL_INT(
		 "result",
		 result,
		 -1 );

		BFIO_TEST_ASSERT_IS_NULL(
		 "file_io_handle",
		 file_io_handle );

		BFIO_TEST_ASSERT_IS_NOT_NULL(
		 "error",
		 error );

		libcerror_error_free(
		 &error );
	}
	/* Test libbfio_file_io_handle_initialize with memset failing
	 */
	bfio_test_memset_attempts_before_fail = 0;

	result = libbfio_file_io_handle_initialize(
	          &file_io_handle,
	          &error );

	if( bfio_test_memset_attempts_before_fail != -1 )
	{
		bfio_test_memset_attempts_before_fail = -1;

		if( file_io_handle != NULL )
		{
			libbfio_file_io_handle_free(
			 &file_io_handle,
			 NULL );
		}
	}
	else
	{
		BFIO_TEST_ASSERT_EQUAL_INT(
		 "result",
		 result,
		 -1 );

		BFIO_TEST_ASSERT_IS_NULL(
		 "file_io_handle",
		 file_io_handle );

		BFIO_TEST_ASSERT_IS_NOT_NULL(
		 "error",
		 error );

		libcerror_error_free(
		 &error );
	}
#endif /* defined( HAVE_BFIO_TEST_MEMORY ) */

	return( 1 );

on_error:
	if( error != NULL )
	{
		libcerror_error_free(
		 &error );
	}
	if( file_io_handle != NULL )
	{
		libbfio_file_io_handle_free(
		 &file_io_handle,
		 NULL );
	}
	return( 0 );
}

/* Tests the libbfio_file_io_handle_free function
 * Returns 1 if successful or 0 if not
 */
int bfio_test_file_io_handle_free(
     void )
{
	libcerror_error_t *error = NULL;
	int result               = 0;

	/* Test error cases
	 */
	result = libbfio_file_io_handle_free(
	          NULL,
	          &error );

	BFIO_TEST_ASSERT_EQUAL_INT(
	 "result",
	 result,
	 -1 );

	BFIO_TEST_ASSERT_IS_NOT_NULL(
	 "error",
	 error );

	libcerror_error_free(
	 &error );

	return( 1 );

on_error:
	if( error != NULL )
	{
		libcerror_error_free(
		 &error );
	}
	return( 0 );
}

/* Tests the libbfio_file_io_handle_open function
 * Returns 1 if successful or 0 if not
 */
int bfio_test_file_io_handle_open(
     const system_character_t *source )
{
	char narrow_source[ 256 ];

	libbfio_file_io_handle_t *file_io_handle = NULL;
	libcerror_error_t *error                 = NULL;
	size_t source_length                     = 0;
	int result                               = 0;

	/* Initialize test
	 */
	result = bfio_test_get_narrow_source(
	          source,
	          narrow_source,
	          256,
	          &error );

	BFIO_TEST_ASSERT_EQUAL_INT(
	 "result",
	 result,
	 1 );

	BFIO_TEST_ASSERT_IS_NULL(
	 "error",
	 error );

	result = libbfio_file_io_handle_initialize(
	          &file_io_handle,
	          &error );

	BFIO_TEST_ASSERT_EQUAL_INT(
	 "result",
	 result,
	 1 );

	BFIO_TEST_ASSERT_IS_NOT_NULL(
	 "file_io_handle",
	 file_io_handle );

	BFIO_TEST_ASSERT_IS_NULL(
	 "error",
	 error );

	source_length = narrow_string_length(
	                 narrow_source );

	result = libbfio_file_io_handle_set_name(
	          file_io_handle,
	          narrow_source,
	          source_length,
	          &error );

	BFIO_TEST_ASSERT_EQUAL_INT(
	 "result",
	 result,
	 1 );

	BFIO_TEST_ASSERT_IS_NULL(
	 "error",
	 error );

	/* Test open
	 */
	result = libbfio_file_io_handle_open(
	          file_io_handle,
	          LIBBFIO_OPEN_READ,
	          &error );

	BFIO_TEST_ASSERT_EQUAL_INT(
	 "result",
	 result,
	 1 );

	BFIO_TEST_ASSERT_IS_NULL(
	 "error",
	 error );

	/* Clean up
	 */
	result = libbfio_file_io_handle_close(
	          file_io_handle,
	          &error );

	BFIO_TEST_ASSERT_EQUAL_INT(
	 "result",
	 result,
	 0 );

	BFIO_TEST_ASSERT_IS_NULL(
	 "error",
	 error );

	result = libbfio_file_io_handle_free(
	          &file_io_handle,
	          &error );

	BFIO_TEST_ASSERT_EQUAL_INT(
	 "result",
	 result,
	 1 );

	BFIO_TEST_ASSERT_IS_NULL(
	 "file_io_handle",
	 file_io_handle );

	BFIO_TEST_ASSERT_IS_NULL(
	 "error",
	 error );

	return( 1 );

on_error:
	if( error != NULL )
	{
		libcerror_error_free(
		 &error );
	}
	if( file_io_handle != NULL )
	{
		libbfio_file_io_handle_free(
		 &file_io_handle,
		 NULL );
	}
	return( 0 );
}

/* Tests the libbfio_file_io_handle_close function
 * Returns 1 if successful or 0 if not
 */
int bfio_test_file_io_handle_close(
     void )
{
	libcerror_error_t *error = NULL;
	int result               = 0;

	/* Test error cases
	 */
	result = libbfio_file_io_handle_close(
	          NULL,
	          &error );

	BFIO_TEST_ASSERT_EQUAL_INT(
	 "result",
	 result,
	 -1 );

	BFIO_TEST_ASSERT_IS_NOT_NULL(
	 "error",
	 error );

	libcerror_error_free(
	 &error );

	return( 1 );

on_error:
	if( error != NULL )
	{
		libcerror_error_free(
		 &error );
	}
	return( 0 );
}

/* Tests the libbfio_file_io_handle_open and libbfio_file_io_handle_close functions
 * Returns 1 if successful or 0 if not
 */
int bfio_test_file_io_handle_open_close(
     const system_character_t *source )
{
	char narrow_source[ 256 ];

	libbfio_file_io_handle_t *file_io_handle = NULL;
	libcerror_error_t *error                 = NULL;
	size_t source_length                     = 0;
	int result                               = 0;

	/* Initialize test
	 */
	result = bfio_test_get_narrow_source(
	          source,
	          narrow_source,
	          256,
	          &error );

	BFIO_TEST_ASSERT_EQUAL_INT(
	 "result",
	 result,
	 1 );

	BFIO_TEST_ASSERT_IS_NULL(
	 "error",
	 error );

	result = libbfio_file_io_handle_initialize(
	          &file_io_handle,
	          &error );

	BFIO_TEST_ASSERT_EQUAL_INT(
	 "result",
	 result,
	 1 );

	BFIO_TEST_ASSERT_IS_NOT_NULL(
	 "file_io_handle",
	 file_io_handle );

	BFIO_TEST_ASSERT_IS_NULL(
	 "error",
	 error );

	source_length = narrow_string_length(
	                 narrow_source );

	result = libbfio_file_io_handle_set_name(
	          file_io_handle,
	          narrow_source,
	          source_length,
	          &error );

	BFIO_TEST_ASSERT_EQUAL_INT(
	 "result",
	 result,
	 1 );

	BFIO_TEST_ASSERT_IS_NULL(
	 "error",
	 error );

	/* Test open and close
	 */
	result = libbfio_file_io_handle_open(
	          file_io_handle,
	          LIBBFIO_OPEN_READ,
	          &error );

	BFIO_TEST_ASSERT_EQUAL_INT(
	 "result",
	 result,
	 1 );

	BFIO_TEST_ASSERT_IS_NULL(
	 "error",
	 error );

	result = libbfio_file_io_handle_close(
	          file_io_handle,
	          &error );

	BFIO_TEST_ASSERT_EQUAL_INT(
	 "result",
	 result,
	 0 );

	BFIO_TEST_ASSERT_IS_NULL(
	 "error",
	 error );

	/* Test open and close a second time to validate clean up on close
	 */
	result = libbfio_file_io_handle_open(
	          file_io_handle,
	          LIBBFIO_OPEN_READ,
	          &error );

	BFIO_TEST_ASSERT_EQUAL_INT(
	 "result",
	 result,
	 1 );

	BFIO_TEST_ASSERT_IS_NULL(
	 "error",
	 error );

	result = libbfio_file_io_handle_close(
	          file_io_handle,
	          &error );

	BFIO_TEST_ASSERT_EQUAL_INT(
	 "result",
	 result,
	 0 );

	BFIO_TEST_ASSERT_IS_NULL(
	 "error",
	 error );

	/* Clean up
	 */
	result = libbfio_file_io_handle_free(
	          &file_io_handle,
	          &error );

	BFIO_TEST_ASSERT_EQUAL_INT(
	 "result",
	 result,
	 1 );

	BFIO_TEST_ASSERT_IS_NULL(
	 "file_io_handle",
	 file_io_handle );

	BFIO_TEST_ASSERT_IS_NULL(
	 "error",
	 error );

	return( 1 );

on_error:
	if( error != NULL )
	{
		libcerror_error_free(
		 &error );
	}
	if( file_io_handle != NULL )
	{
		libbfio_file_io_handle_free(
		 &file_io_handle,
		 NULL );
	}
	return( 0 );
}

/* Tests the libbfio_file_io_handle_get_size function
 * Returns 1 if successful or 0 if not
 */
int bfio_test_file_io_handle_get_size(
     libbfio_file_io_handle_t *file_io_handle )
{
	libcerror_error_t *error = NULL;
	size64_t size            = 0;
	int result               = 0;

	/* Test regular cases
	 */
	result = libbfio_file_io_handle_get_size(
	          file_io_handle,
	          &size,
	          &error );

	BFIO_TEST_ASSERT_EQUAL_INT(
	 "result",
	 result,
	 1 );

	BFIO_TEST_ASSERT_IS_NULL(
	 "error",
	 error );

	/* Test error cases
	 */
	result = libbfio_file_io_handle_get_size(
	          NULL,
	          &size,
	          &error );

	BFIO_TEST_ASSERT_EQUAL_INT(
	 "result",
	 result,
	 -1 );

	BFIO_TEST_ASSERT_IS_NOT_NULL(
	 "error",
	 error );

	libcerror_error_free(
	 &error );

	result = libbfio_file_io_handle_get_size(
	          file_io_handle,
	          NULL,
	          &error );

	BFIO_TEST_ASSERT_EQUAL_INT(
	 "result",
	 result,
	 -1 );

	BFIO_TEST_ASSERT_IS_NOT_NULL(
	 "error",
	 error );

	libcerror_error_free(
	 &error );

	return( 1 );

on_error:
	if( error != NULL )
	{
		libcerror_error_free(
		 &error );
	}
	return( 0 );
}

#endif /* defined( __GNUC__ ) && !defined( LIBBFIO_DLL_IMPORT ) */

/* The main program
 */
#if defined( HAVE_WIDE_SYSTEM_CHARACTER )
int wmain(
     int argc,
     wchar_t * const argv[] )
#else
int main(
     int argc,
     char * const argv[] )
#endif
{
#if defined( __GNUC__ ) && !defined( LIBBFIO_DLL_IMPORT )
	char narrow_source[ 256 ];

	libbfio_file_io_handle_t *file_io_handle = NULL;
	size_t source_length                     = 0;
#endif

	libcerror_error_t *error                 = NULL;
	system_character_t *source               = NULL;
	system_integer_t option                  = 0;
	int result                               = 0;

	while( ( option = bfio_test_getopt(
	                   argc,
	                   argv,
	                   _SYSTEM_STRING( "" ) ) ) != (system_integer_t) -1 )
	{
		switch( option )
		{
			case (system_integer_t) '?':
			default:
				fprintf(
				 stderr,
				 "Invalid argument: %" PRIs_SYSTEM ".\n",
				 argv[ optind - 1 ] );

				return( EXIT_FAILURE );
		}
	}
	if( optind < argc )
	{
		source = argv[ optind ];
	}
#if defined( HAVE_DEBUG_OUTPUT ) && defined( BFIO_TEST_FILE_VERBOSE )
	libbfio_notify_set_verbose(
	 1 );
	libbfio_notify_set_stream(
	 stderr,
	 NULL );
#endif

#if defined( __GNUC__ ) && !defined( LIBBFIO_DLL_IMPORT )

	BFIO_TEST_RUN(
	 "libbfio_file_io_handle_initialize",
	 bfio_test_file_io_handle_initialize );

	BFIO_TEST_RUN(
	 "libbfio_file_io_handle_free",
	 bfio_test_file_io_handle_free );

/* TODO add test for libbfio_file_io_handle_clone */

#if !defined( __BORLANDC__ ) || ( __BORLANDC__ >= 0x0560 )
	if( source != NULL )
	{
		BFIO_TEST_RUN_WITH_ARGS(
		 "libbfio_file_io_handle_open",
		 bfio_test_file_io_handle_open,
		 source );

		BFIO_TEST_RUN(
		 "libbfio_file_io_handle_close",
		 bfio_test_file_io_handle_close );

		BFIO_TEST_RUN_WITH_ARGS(
		 "libbfio_file_io_handle_open_close",
		 bfio_test_file_io_handle_open_close,
		 source );

		/* Initialize test
		 */
		result = bfio_test_get_narrow_source(
		          source,
		          narrow_source,
		          256,
		          &error );

		BFIO_TEST_ASSERT_EQUAL_INT(
		 "result",
		 result,
		 1 );

		BFIO_TEST_ASSERT_IS_NULL(
		 "error",
		 error );

		result = libbfio_file_io_handle_initialize(
		          &file_io_handle,
		          &error );

		BFIO_TEST_ASSERT_EQUAL_INT(
		 "result",
		 result,
		 1 );

		BFIO_TEST_ASSERT_IS_NOT_NULL(
		 "file_io_handle",
		 file_io_handle );

		BFIO_TEST_ASSERT_IS_NULL(
		 "error",
		 error );

		source_length = narrow_string_length(
		                 narrow_source );

		result = libbfio_file_io_handle_set_name(
		          file_io_handle,
		          narrow_source,
		          source_length,
		          &error );

		BFIO_TEST_ASSERT_EQUAL_INT(
		 "result",
		 result,
		 1 );

		BFIO_TEST_ASSERT_IS_NULL(
		 "error",
		 error );

		result = libbfio_file_io_handle_open(
		          file_io_handle,
		          LIBBFIO_OPEN_READ,
		          &error );

		BFIO_TEST_ASSERT_EQUAL_INT(
		 "result",
		 result,
		 1 );

		BFIO_TEST_ASSERT_IS_NULL(
		 "error",
		 error );

/* TODO add test for libbfio_file_io_handle_get_name_size */

/* TODO add test for libbfio_file_io_handle_get_name */

/* TODO add test for libbfio_file_io_handle_set_name */

#if defined( HAVE_WIDE_CHARACTER_TYPE )

/* TODO add test for libbfio_file_io_handle_get_name_size_wide */

/* TODO add test for libbfio_file_io_handle_get_name_wide */

/* TODO add test for libbfio_file_io_handle_set_name_wide */

#endif /* defined( HAVE_WIDE_CHARACTER_TYPE ) */

/* TODO add test for libbfio_file_io_handle_read */

/* TODO add test for libbfio_file_io_handle_seek_offset */

/* TODO add test for libbfio_file_io_handle_exists */

/* TODO add test for libbfio_file_io_handle_is_open */

		BFIO_TEST_RUN_WITH_ARGS(
		 "libbfio_file_io_handle_get_size",
		 bfio_test_file_io_handle_get_size,
		 file_io_handle );

		/* Clean up
		 */
		result = libbfio_file_io_handle_close(
		          file_io_handle,
		          &error );

		BFIO_TEST_ASSERT_EQUAL_INT(
		 "result",
		 result,
		 0 );

		BFIO_TEST_ASSERT_IS_NULL(
		 "error",
		 error );

		result = libbfio_file_io_handle_free(
		          &file_io_handle,
		          &error );

		BFIO_TEST_ASSERT_EQUAL_INT(
		 "result",
		 result,
		 1 );

		BFIO_TEST_ASSERT_IS_NULL(
		 "file_io_handle",
		 file_io_handle );

		BFIO_TEST_ASSERT_IS_NULL(
		 "error",
		 error );
	}
#endif /* !defined( __BORLANDC__ ) || ( __BORLANDC__ >= 0x0560 ) */

/* TODO add test for libbfio_file_io_handle_write */

#endif /* defined( __GNUC__ ) && !defined( LIBBFIO_DLL_IMPORT ) */

	return( EXIT_SUCCESS );

on_error:
	if( error != NULL )
	{
		libcerror_error_free(
		 &error );
	}
#if defined( __GNUC__ ) && !defined( LIBBFIO_DLL_IMPORT )
	if( file_io_handle != NULL )
	{
		libbfio_file_io_handle_free(
		 &file_io_handle,
		 NULL );
	}
#endif /* defined( __GNUC__ ) && !defined( LIBBFIO_DLL_IMPORT ) */

	return( EXIT_FAILURE );
}

