/*
 * Path functions
 *
 * Copyright (c) 2009-2011, Joachim Metz <jbmetz@users.sourceforge.net>
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
#include <types.h>

#include <libcstring.h>
#include <liberror.h>

#if defined( HAVE_ERRNO_H ) || defined( WINAPI )
#include <errno.h>
#endif

#if defined( HAVE_UNISTD_H )
#include <unistd.h>
#endif

#include "libbfio_error_string.h"
#include "libbfio_narrow_split_string.h"
#include "libbfio_narrow_string.h"
#include "libbfio_path.h"
#include "libbfio_wide_split_string.h"
#include "libbfio_wide_string.h"

#if defined( WINAPI )
enum LIBBFIO_PATH_TYPES
{
	LIBBFIO_PATH_TYPE_ABSOLUTE,
	LIBBFIO_PATH_TYPE_DEVICE,
	LIBBFIO_PATH_TYPE_EXTENDED_LENGTH,
	LIBBFIO_PATH_TYPE_RELATIVE,
	LIBBFIO_PATH_TYPE_UNC
};

#else
enum LIBBFIO_PATH_TYPES
{
	LIBBFIO_PATH_TYPE_ABSOLUTE,
	LIBBFIO_PATH_TYPE_RELATIVE
};

#endif

/* Retrieves the current working directory
 * Returns 1 if successful or -1 on error
 */
int libbfio_path_get_current_working_directory(
     char **current_working_directory,
     size_t *current_working_directory_size,
     liberror_error_t **error )
{
	libcstring_system_character_t error_string[ 128 ];

	static char *function                     = "libbfio_path_get_current_working_directory";

#if defined( WINAPI ) && !defined( USE_CRT_FUNCTIONS )
	DWORD safe_current_working_directory_size = 0;
	DWORD error_code                          = 0;
#endif

	if( current_working_directory == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid current working directory.",
		 function );

		return( -1 );
	}
	if( *current_working_directory != NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid current working directory value already set.",
		 function );

		return( -1 );
	}
	if( current_working_directory_size == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid current working directory size.",
		 function );

		return( -1 );
	}
#if defined( WINAPI ) && !defined( USE_CRT_FUNCTIONS )
	safe_current_working_directory_size = GetCurrentDirectoryA(
	                                       0,
	                                       NULL );

	if( safe_current_working_directory_size == 0 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve current working directory size.",
		 function );

		goto on_error;
	}
	if( (size_t) safe_current_working_directory_size > (size_t) SSIZE_MAX )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: current working directory size value out of bounds.",
		 function );

		goto on_error;
	}
	*current_working_directory_size = (size_t) safe_current_working_directory_size;

#elif defined( WINAPI )
	*current_working_directory_size = (size_t) _MAX_PATH;

#else
	*current_working_directory_size = (size_t) PATH_MAX;

#endif

	*current_working_directory = libcstring_narrow_string_allocate(
	                              *current_working_directory_size );

	if( *current_working_directory == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_MEMORY,
		 LIBERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create current working directory.",
		 function );

		goto on_error;
	}
#if defined( WINAPI ) && !defined( USE_CRT_FUNCTIONS )
	if( GetCurrentDirectoryA(
	     safe_current_working_directory_size,
	     *current_working_directory ) != ( safe_current_working_directory_size - 1 ) )
	{
		error_code = GetLastError();

		if( libbfio_error_string_copy_from_error_number(
		     error_string,
		     128,
		     error_code,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve current working directory with error: %" PRIs_LIBCSTRING_SYSTEM "",
			 function,
			 error_string );
		}
		else
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve current working directory.",
			 function );
		}
		goto on_error;
	}
#elif defined( WINAPI )
	if( _getcwd(
	     *current_working_directory,
	     *current_working_directory_size ) == NULL )
	{
		if( libbfio_error_string_copy_from_error_number(
		     error_string,
		     128,
		     errno,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve current working directory with error: %" PRIs_LIBCSTRING_SYSTEM "",
			 function,
			 error_string );
		}
		else
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve current working directory.",
			 function );
		}
		goto on_error;
	}
#else
	if( getcwd(
	     *current_working_directory,
	     *current_working_directory_size ) == NULL )
	{
		if( libbfio_error_string_copy_from_error_number(
		     error_string,
		     128,
		     errno,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve current working directory with error: %" PRIs_LIBCSTRING_SYSTEM "",
			 function,
			 error_string );
		}
		else
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve current working directory.",
			 function );
		}
		goto on_error;
	}
#endif
	return( 1 );

on_error:
	if( current_working_directory != NULL )
	{
		memory_free(
		 *current_working_directory );

		*current_working_directory = NULL;
	}
	*current_working_directory_size = 0;

	return( -1 );
}

#if defined( WINAPI )

/* Determines the full path of the Windows path specified
 * The function uses the extended-length path format
 * (path with \\?\ prefix)
 *
 * Scenario's that are considered full paths:
 * Device path:			\\.\PhysicalDrive0
 * Extended-length path:	\\?\C:\directory\file.txt
 * Extended-length UNC path:	\\?\UNC\server\share\directory\file.txt
 *
 * Scenario's that are not considered full paths:
 * Local 'absolute' path:	\directory\file.txt
 * Local 'relative' path:	..\directory\file.txt
 * Local 'relative' path:	.\directory\file.txt
 * Volume 'absolute' path:	C:\directory\file.txt
 * Volume 'relative' path:	C:directory\file.txt
 * UNC path:			\\server\share\directory\file.txt
 *
 * This function does not support paths like (although Windows does):
 * C:\..\directory\file.txt
 *
 * Returns 1 if succesful or -1 on error
 */
int libbfio_path_get_full_path(
     const char *path,
     size_t path_length,
     char **full_path,
     size_t *full_path_size,
     liberror_error_t **error )
{
	libbfio_narrow_split_string_t *current_directory_split_string = NULL;
	libbfio_narrow_split_string_t *path_split_string              = NULL;
	char *change_volume_name                                      = NULL;
	char *current_working_directory                               = NULL;
	char *current_directory                                       = NULL;
	char *current_directory_string_segment                        = NULL;
	char *full_path_prefix                                        = NULL;
	char *last_used_path_string_segment                           = NULL;
	char *path_string_segment                                     = NULL;
	char *volume_name                                             = NULL;
	static char *function                                         = "libbfio_path_get_full_path";
	size_t current_directory_name_index                           = 0;
	size_t current_directory_size                                 = 0;
	size_t current_directory_string_segment_size                  = 0;
	size_t current_working_directory_size                         = 0;
	size_t full_path_index                                        = 0;
	size_t full_path_prefix_length                                = 0;
	size_t last_used_path_string_segment_size                     = 0;
	size_t path_directory_name_index                              = 0;
	size_t path_string_segment_size                               = 0;
	size_t share_name_index                                       = 0;
	size_t volume_name_length                                     = 0;
	uint8_t path_type                                             = LIBBFIO_PATH_TYPE_RELATIVE;
	int current_directory_number_of_segments                      = 0;
	int current_directory_segment_index                           = 0;
	int error_abort                                               = 0;
	int last_used_path_segment_index                              = 0;
	int path_number_of_segments                                   = 0;
	int path_segment_index                                        = 0;

	if( path == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid path.",
		 function );

		return( -1 );
	}
	if( path_length == 0 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_ZERO_OR_LESS,
		 "%s: invalid path length is zero.",
		 function );

		return( -1 );
	}
	if( path_length >= (size_t) SSIZE_MAX )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid path length value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( full_path == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid full path.",
		 function );

		return( -1 );
	}
	if( *full_path != NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid full path value already set.",
		 function );

		return( -1 );
	}
	if( full_path_size == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid full path size.",
		 function );

		return( -1 );
	}
	if( path_length >= 2 )
	{
		/* Check if the path starts with a volume letter
		 */
		if( ( path[ 1 ] == ':' )
		 && ( ( ( path[ 0 ] >= 'A' )
		   && ( path[ 0 ] <= 'Z' ) )
		  || ( ( path[ 0 ] >= 'a' )
		   && ( path[ 0 ] <= 'z' ) ) ) )
		{
			volume_name               = &( path[ 0 ] );
			volume_name_length        = 2;
			path_directory_name_index = 2;

			if( ( path_length >= 3 )
			 && ( path[ 2 ] == '\\' ) )
			{
				path_type                  = LIBBFIO_PATH_TYPE_ABSOLUTE;
				path_directory_name_index += 1;
			}
		}
		/* Check for special paths
		 * paths with prefix: \\
		 */
		else if( ( path[ 0 ] == '\\' )
		      && ( path[ 1 ] == '\\' ) )
		{
			/* Determine if the path is a special path
			 * device path prefix:          \\.\
			 * extended-length path prefix: \\?\
			 */
			if( ( path_length >= 4 )
			 && ( ( path[ 2 ] == '.' )
			   || ( path[ 2 ] == '?' ) )
			 && ( path[ 3 ] == '\\' ) )
			{
				if( path[ 2 ] == '.' )
				{
					path_type = LIBBFIO_PATH_TYPE_DEVICE;
				}
				else
				{
					path_type = LIBBFIO_PATH_TYPE_EXTENDED_LENGTH;
				}
			}
			else
			{
				/* Determine the volume in an UNC path
				 * \\server\share
				 */
				for( share_name_index = 2;
				     share_name_index <= path_length;
				     share_name_index++ )
				{
					if( path[ share_name_index ] == '\\' )
					{
						break;
					}
				}
				if( share_name_index > path_length )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
					 "%s: invalid path - missing share name.",
					 function );

					goto on_error;
				}
				for( path_directory_name_index = share_name_index + 1;
				     path_directory_name_index <= path_length;
				     path_directory_name_index++ )
				{
					if( path[ path_directory_name_index ] == '\\' )
					{
						break;
					}
				}
				path_type          = LIBBFIO_PATH_TYPE_UNC;
				volume_name        = &( path[ 2 ] );
				volume_name_length = path_directory_name_index - 2;
			}
		}
	}
	/* If the path is a device path, an extended-length path or an UNC
	 * do not bother to lookup the current directory
	 */
	if( ( path_type != LIBBFIO_PATH_TYPE_DEVICE )
	 && ( path_type != LIBBFIO_PATH_TYPE_EXTENDED_LENGTH )
	 && ( path_type != LIBBFIO_PATH_TYPE_UNC ) )
	{
		/* If the path contains a volume name switch to that
		 * volume to determine the current directory
		 */
		if( volume_name != NULL )
		{
			if( libbfio_path_get_current_working_directory(
			     &current_working_directory,
			     &current_working_directory_size,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve current working directory.",
				 function );

				goto on_error;
			}
			change_volume_name = libcstring_narrow_string_allocate(
			                      volume_name_length + 1 );

			if( change_volume_name == NULL )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_MEMORY,
				 LIBERROR_MEMORY_ERROR_INSUFFICIENT,
				 "%s: unable to create change volume name.",
				 function );

				goto on_error;
			}
			if( libcstring_narrow_string_copy(
			     change_volume_name,
			     volume_name,
			     volume_name_length ) == NULL )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_MEMORY,
				 LIBERROR_MEMORY_ERROR_COPY_FAILED,
				 "%s: unable to set change volume name.",
				 function );

				goto on_error;
			}
			change_volume_name[ volume_name_length ] = 0;

#if defined( WINAPI ) && !defined( USE_CRT_FUNCTIONS )
/* TODO add pre Windows XP support */
			if( SetCurrentDirectoryA(
			     change_volume_name ) == 0 )
			{
				error_code = GetLastError();

				if( libbfio_error_string_copy_from_error_number(
				     error_string,
				     128,
				     error_code,
				     error ) != 1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_SET_FAILED,
					 "%s: unable to change current working directory with error: %" PRIs_LIBCSTRING_SYSTEM "",
					 function,
					 error_string );
				}
				else
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_SET_FAILED,
					 "%s: unable to change current working directory.",
					 function );
				}
				goto on_error;
			}
#else
			if( _chdir(
			     change_volume_name ) != 0 )
			{
				if( libbfio_error_string_copy_from_error_number(
				     error_string,
				     128,
				     errno,
				     error ) != 1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_SET_FAILED,
					 "%s: unable to change current working directory with error: %" PRIs_LIBCSTRING_SYSTEM "",
					 function,
					 error_string );
				}
				else
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_SET_FAILED,
					 "%s: unable to change current working directory.",
					 function );
				}
				goto on_error;
			}
#endif
			memory_free(
			 change_volume_name );

			change_volume_name = NULL;
		}
		if( libbfio_path_get_current_working_directory(
		     &current_directory,
		     &current_directory_size,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve current directory.",
			 function );

			error_abort = 1;
		}
		if( current_working_directory != NULL )
		{
#if defined( WINAPI ) && !defined( USE_CRT_FUNCTIONS )
/* TODO add pre Windows XP support */
			if( SetCurrentDirectoryA(
			     current_working_directory ) == 0 )
			{
				error_code = GetLastError();

				if( libbfio_error_string_copy_from_error_number(
				     error_string,
				     128,
				     error_code,
				     error ) != 1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_SET_FAILED,
					 "%s: unable to change current working directory with error: %" PRIs_LIBCSTRING_SYSTEM "",
					 function,
					 error_string );
				}
				else
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_SET_FAILED,
					 "%s: unable to change current working directory.",
					 function );
				}
				goto on_error;
			}
#else
			if( _chdir(
			     current_working_directory ) != 0 )
			{
				if( libbfio_error_string_copy_from_error_number(
				     error_string,
				     128,
				     errno,
				     error ) != 1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_SET_FAILED,
					 "%s: unable to change current working directory with error: %" PRIs_LIBCSTRING_SYSTEM "",
					 function,
					 error_string );
				}
				else
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_SET_FAILED,
					 "%s: unable to change current working directory.",
					 function );
				}
				goto on_error;
			}
#endif
			memory_free(
			 current_working_directory );

			current_working_directory = NULL;
		}
		/* Make sure the current working directory has been changed
		 * back to its original value
		 */
		if( error_abort != 0 )
		{
			goto on_error;
		}
		/* Determine the volume name using the current directory if necessary
		 */
		if( current_directory_size >= 3 )
		{
			/* Check if the path starts with a volume letter
			 */
			if( ( current_directory[ 1 ] == ':' )
			 && ( ( ( current_directory[ 0 ] >= 'A' )
			   && ( current_directory[ 0 ] <= 'Z' ) )
			  || ( ( current_directory[ 0 ] >= 'a' )
			   && ( current_directory[ 0 ] <= 'z' ) ) ) )
			{
				if( volume_name == NULL )
				{
					volume_name        = &( current_directory[ 0 ] );
					volume_name_length = 2;
				}
				current_directory_name_index = 2;

				if( current_directory[ 2 ] == '\\' )
				{
					current_directory_name_index += 1;
				}
			}
			/* Check for special paths
			 * paths with prefix: \\
			 */
			else if( ( current_directory[ 0 ] == '\\' )
			      && ( current_directory[ 1 ] == '\\' ) )
			{
				/* Ignore the following special paths
				 * device path prefix:          \\.\
				 */
				if( ( current_directory_size >= 4 )
				 && ( current_directory[ 2 ] == '.' )
				 && ( current_directory[ 3 ] == '\\' ) )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
					 LIBERROR_ARGUMENT_ERROR_UNSUPPORTED_VALUE,
					 "%s: unsupported current directory.",
					 function );

					return( -1 );
				}
				else
				{
					/* Determine the volume in an UNC path
					 * \\server\share
					 */
					for( share_name_index = 2;
					     share_name_index < current_directory_size;
					     share_name_index++ )
					{
						if( current_directory[ share_name_index ] == '\\' )
						{
							break;
						}
					}
					if( share_name_index >= current_directory_size )
					{
						liberror_error_set(
						 error,
						 LIBERROR_ERROR_DOMAIN_RUNTIME,
						 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
						 "%s: invalid path - missing share name.",
						 function );

						goto on_error;
					}
					for( current_directory_name_index = share_name_index + 1;
					     current_directory_name_index < current_directory_size;
					     current_directory_name_index++ )
					{
						if( current_directory[ current_directory_name_index ] == '\\' )
						{
							break;
						}
					}
					if( volume_name == NULL )
					{
						volume_name        = &( current_directory[ 2 ] );
						volume_name_length = current_directory_name_index - 2;
					}
				}
			}
		}
	}
	if( volume_name == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
		 "%s: invalid volume name value missing.",
		 function );

		goto on_error;
	}
	if( current_directory != NULL )
	{
		if( libbfio_narrow_string_split(
		     &( current_directory[ current_directory_name_index ] ),
		     current_directory_size - ( current_directory_name_index + 1 ) + 1,
		     '\\',
		     &current_directory_split_string,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to split current directory.",
			 function );

			goto on_error;
		}
	}
	if( libbfio_narrow_string_split(
	     &( path[ path_directory_name_index ] ),
	     path_length - path_directory_name_index + 1,
	     '\\',
	     &path_split_string,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to split path.",
		 function );

		goto on_error;
	}
	/* The size of the full path consists of:
	 * the size of the prefix (\\?\ or \\.\)
	 * the length of the volume name
	 * a directory separator
	 */
	*full_path_size = 5 + volume_name_length;

	/* If the path contains an UNC path
	 * add the size of the UNC\ prefix
	 */
	if( share_name_index > 0 )
	{
		*full_path_size += 4;
	}
	/* If the path is relative
	 * add the size of the current directory
	 * a directory separator, if necessary
	 */
	if( path_type == LIBBFIO_PATH_TYPE_RELATIVE )
	{
		*full_path_size += ( current_directory_size - ( current_directory_name_index + 1 ) );

		if( ( current_directory_size >= 2 )
		 && ( current_directory[ current_directory_size - 2 ] != '\\' ) )
		{
			*full_path_size += 1;
		}
	}
	if( current_directory_split_string != NULL )
	{
		if( libbfio_narrow_split_string_get_number_of_segments(
		     current_directory_split_string,
		     &current_directory_number_of_segments,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve number of current directory string segments.",
			 function );

			goto on_error;
		}
		current_directory_segment_index = current_directory_number_of_segments - 1;
	}
	if( libbfio_narrow_split_string_get_number_of_segments(
	     path_split_string,
	     &path_number_of_segments,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of path string segments.",
		 function );

		goto on_error;
	}
	for( path_segment_index = 0;
	     path_segment_index < path_number_of_segments;
	     path_segment_index++ )
	{
		if( libbfio_narrow_split_string_get_segment_by_index(
		     path_split_string,
		     path_segment_index,
		     &path_string_segment,
		     &path_string_segment_size,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve path string segment: %d.",
			 function,
			 path_segment_index );

			goto on_error;
		}
		if( path_string_segment == NULL )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: missing path string segment: %d.",
			 function,
			 path_segment_index );

			goto on_error;
		}
		/* If the path is .. reverse the current path by one directory
		 */
		if( ( path_string_segment_size == 3 )
		 && ( path_string_segment[ 0 ] == '.' )
		 && ( path_string_segment[ 1 ] == '.' ) )
		{
			if( last_used_path_segment_index == 0 )
			{
				if( libbfio_narrow_split_string_get_segment_by_index(
				     current_directory_split_string,
				     current_directory_segment_index,
				     &current_directory_string_segment,
				     &current_directory_string_segment_size,
				     error ) != 1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_GET_FAILED,
					 "%s: unable to retrieve current directory string segment: %d.",
					 function,
					 current_directory_segment_index );

					goto on_error;
				}
				if( current_directory_string_segment == NULL )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
					 "%s: missing current directory string segment: %d.",
					 function,
					 current_directory_segment_index );

					goto on_error;
				}
				/* Remove the size of the parent directory name and a directory separator
				 * Note that the size includes the end of string character
				 */
				*full_path_size -= current_directory_string_segment_size;

				if( libbfio_narrow_split_string_set_segment_by_index(
				     current_directory_split_string,
				     current_directory_segment_index,
				     NULL,
				     0,
				     error ) != 1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_SET_FAILED,
					 "%s: unable to set current directory string segment: %d.",
					 function,
					 current_directory_segment_index );

					goto on_error;
				}
				current_directory_segment_index--;
			}
			else
			{
				if( libbfio_narrow_split_string_get_segment_by_index(
				     path_split_string,
				     last_used_path_segment_index,
				     &last_used_path_string_segment,
				     &last_used_path_string_segment_size,
				     error ) != 1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_GET_FAILED,
					 "%s: unable to retrieve last used path string segment: %d.",
					 function,
					 last_used_path_segment_index );

					goto on_error;
				}
				if( last_used_path_string_segment == NULL )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
					 "%s: missing last used path string string segment: %d.",
					 function,
					 last_used_path_segment_index );

					goto on_error;
				}
				/* Remove the size of the parent directory name and a directory separator
				 * Note that the size includes the end of string character
				 */
				*full_path_size -= last_used_path_string_segment_size;

				if( libbfio_narrow_split_string_set_segment_by_index(
				     path_split_string,
				     path_segment_index,
				     NULL,
				     0,
				     error ) != 1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_SET_FAILED,
					 "%s: unable to set path string segment: %d.",
					 function,
					 path_segment_index );

					goto on_error;
				}
				/* Find the previous path split value that contains a name
				 */
				while( last_used_path_segment_index > 0 )
				{
					last_used_path_segment_index--;

					if( libbfio_narrow_split_string_get_segment_by_index(
					     path_split_string,
					     last_used_path_segment_index,
					     &last_used_path_string_segment,
					     &last_used_path_string_segment_size,
					     error ) != 1 )
					{
						liberror_error_set(
						 error,
						 LIBERROR_ERROR_DOMAIN_RUNTIME,
						 LIBERROR_RUNTIME_ERROR_GET_FAILED,
						 "%s: unable to retrieve last used path string segment: %d.",
						 function,
						 last_used_path_segment_index );

						goto on_error;
					}
					if( last_used_path_string_segment_size != 0 )
					{
						break;
					}
				}
			}
			if( libbfio_narrow_split_string_set_segment_by_index(
			     path_split_string,
			     path_segment_index,
			     NULL,
			     0,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_SET_FAILED,
				 "%s: unable to set path string segment: %d.",
				 function,
				 path_segment_index );

				goto on_error;
			}
		}
		/* If the path is . ignore the entry
		 */
		else if( ( path_string_segment_size == 2 )
		      && ( path_string_segment[ 0 ] == '.' ) )
		{
			if( libbfio_narrow_split_string_set_segment_by_index(
			     path_split_string,
			     path_segment_index,
			     NULL,
			     0,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_SET_FAILED,
				 "%s: unable to set path string segment: %d.",
				 function,
				 path_segment_index );

				goto on_error;
			}
		}
		else
		{
			/* Add the size of the directory or file name and a directory separator
			 * Note that the size includes the end of string character
			 */
			*full_path_size += path_string_segment_size;

			last_used_path_segment_index = path_segment_index;
		}
	}
	/* Note that the last path separator serves as the end of string
	 */
	full_path_index = 0;

	*full_path = libcstring_narrow_string_allocate(
	              *full_path_size );

	if( *full_path == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_MEMORY,
		 LIBERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create full path.",
		 function );

		goto on_error;
	}
	if( path_type == LIBBFIO_PATH_TYPE_DEVICE )
	{
		full_path_prefix        = "\\\\.\\";
		full_path_prefix_length = 4;
	}
	else
	{
		full_path_prefix        = "\\\\?\\";
		full_path_prefix_length = 4;
	}
	if( libcstring_narrow_string_copy(
	     &( ( *full_path )[ full_path_index ] ),
	     full_path_prefix,
	     full_path_prefix_length ) == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set prefix in full path.",
		 function );

		goto on_error;
	}
	full_path_index += full_path_prefix_length;

	/* If there is a share name the path is an UNC path
	 */
	if( share_name_index > 0 )
	{
		if( libcstring_narrow_string_copy(
		     &( ( *full_path )[ full_path_index ] ),
		     "UNC\\",
		     4 ) == NULL )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_SET_FAILED,
			 "%s: unable to set UNC\\ prefix in full path.",
			 function );

			goto on_error;
		}
		full_path_index += 4;
	}
	if( libcstring_narrow_string_copy(
	     &( ( *full_path )[ full_path_index ] ),
	     volume_name,
	     volume_name_length ) == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_SET_FAILED,
		 "%s: unable to set volume name in full path.",
		 function );

		goto on_error;
	}
	full_path_index += volume_name_length;

	( *full_path )[ full_path_index ] = '\\';

	full_path_index += 1;

	/* If the path is relative
	 * add the current directory elements
	 */
	if( ( path_type == LIBBFIO_PATH_TYPE_RELATIVE )
	 && ( current_directory_split_string != NULL ) )
	{
		for( current_directory_segment_index = 0;
		     current_directory_segment_index < current_directory_number_of_segments;
		     current_directory_segment_index++ )
		{
			if( libbfio_narrow_split_string_get_segment_by_index(
			     current_directory_split_string,
			     current_directory_segment_index,
			     &current_directory_string_segment,
			     &current_directory_string_segment_size,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve current directory string segment: %d.",
				 function,
				 current_directory_segment_index );

				goto on_error;
			}
			if( current_directory_string_segment_size != 0 )
			{
				if( current_directory_string_segment == NULL )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
					 "%s: missing current directory string segment: %d.",
					 function,
					 current_directory_segment_index );

					goto on_error;
				}
				if( libcstring_narrow_string_copy(
				     &( ( *full_path )[ full_path_index ] ),
				     current_directory_string_segment,
				     current_directory_string_segment_size - 1 ) == NULL )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_SET_FAILED,
					 "%s: unable to set current directory split value: %d in full path.",
					 function,
					 current_directory_segment_index );

					goto on_error;
				}
				full_path_index += current_directory_string_segment_size - 1;

				( *full_path )[ full_path_index ] = '\\';

				full_path_index += 1;
			}
		}
	}
	for( path_segment_index = 0;
	     path_segment_index < path_number_of_segments;
	     path_segment_index++ )
	{
		if( libbfio_narrow_split_string_get_segment_by_index(
		     path_split_string,
		     path_segment_index,
		     &path_string_segment,
		     &path_string_segment_size,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve path string segment: %d.",
			 function,
			 path_segment_index );

			goto on_error;
		}
		if( path_string_segment_size != 0 )
		{
			if( path_string_segment == NULL )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
				 "%s: missing path string segment: %d.",
				 function,
				 path_segment_index );

				goto on_error;
			}
			if( libcstring_narrow_string_copy(
			     &( ( *full_path )[ full_path_index ] ),
			     path_string_segment,
			     path_string_segment_size - 1 ) == NULL )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_SET_FAILED,
				 "%s: unable to set path split value: %d in full path.",
				 function,
				 path_segment_index );

				goto on_error;
			}
			full_path_index += path_string_segment_size - 1;

			( *full_path )[ full_path_index ] = '\\';

			full_path_index += 1;
		}
	}
	( *full_path )[ full_path_index - 1 ] = 0;

	if( libbfio_narrow_split_string_free(
	     &path_split_string,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_FINALIZE_FAILED,
		 "%s: unable to free path split string.",
		 function );

		goto on_error;
	}
	if( current_directory_split_string != NULL )
	{
		if( libbfio_narrow_split_string_free(
		     &current_directory_split_string,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free current directory split string.",
			 function );

			goto on_error;
		}
	}
	if( current_directory != NULL )
	{
		memory_free(
		 current_directory );
	}
	return( 1 );

on_error:
	if( *full_path != NULL )
	{
		memory_free(
		 *full_path );

		*full_path = NULL;
	}
	*full_path_size = 0;

	if( path_split_string != NULL )
	{
		libbfio_narrow_split_string_free(
		 &path_split_string,
		 NULL );
	}
	if( current_directory_split_string != NULL )
	{
		libbfio_narrow_split_string_free(
		 &current_directory_split_string,
		 NULL );
	}
	if( current_directory != NULL )
	{
		memory_free(
		 current_directory );
	}
	if( change_volume_name != NULL )
	{
		memory_free(
		 change_volume_name );
	}
	if( current_working_directory != NULL )
	{
		memory_free(
		 current_working_directory );
	}
	return( -1 );
}

#else

/* Determines the full path of the POSIX path specified
 * Returns 1 if succesful or -1 on error
 */
int libbfio_path_get_full_path(
     const char *path,
     size_t path_length,
     char **full_path,
     size_t *full_path_size,
     liberror_error_t **error )
{
	libbfio_narrow_split_string_t *current_directory_split_string = NULL;
	libbfio_narrow_split_string_t *path_split_string              = NULL;
	char *current_directory                                       = NULL;
	char *current_directory_string_segment                        = NULL;
	char *last_used_path_string_segment                           = NULL;
	char *path_string_segment                                     = NULL;
	static char *function                                         = "libbfio_path_get_full_path";
	size_t current_directory_size                                 = 0;
	size_t current_directory_string_segment_size                  = 0;
	size_t full_path_index                                        = 0;
	size_t last_used_path_string_segment_size                     = 0;
	size_t path_string_segment_size                               = 0;
	uint8_t path_type                                             = LIBBFIO_PATH_TYPE_RELATIVE;
	int current_directory_number_of_segments                      = 0;
	int current_directory_segment_index                           = 0;
	int last_used_path_segment_index                              = 0;
	int path_number_of_segments                                   = 0;
	int path_segment_index                                        = 0;

	if( path == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid path.",
		 function );

		return( -1 );
	}
	if( path_length == 0 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_ZERO_OR_LESS,
		 "%s: invalid path length is zero.",
		 function );

		return( -1 );
	}
	if( path_length >= (size_t) SSIZE_MAX )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_VALUE_EXCEEDS_MAXIMUM,
		 "%s: invalid path length value exceeds maximum.",
		 function );

		return( -1 );
	}
	if( full_path == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid full path.",
		 function );

		return( -1 );
	}
	if( *full_path != NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid full path value already set.",
		 function );

		return( -1 );
	}
	if( full_path_size == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid full path size.",
		 function );

		return( -1 );
	}
	if( path[ 0 ] == '/' )
	{
		path_type = LIBBFIO_PATH_TYPE_ABSOLUTE;
	}
	else
	{
		if( libbfio_path_get_current_working_directory(
		     &current_directory,
		     &current_directory_size,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve current directory.",
			 function );

			goto on_error;
		}
	}
	if( current_directory != NULL )
	{
		if( libbfio_narrow_string_split(
		     current_directory,
		     current_directory_size,
		     '/',
		     &current_directory_split_string,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
			 "%s: unable to split current directory.",
			 function );

			goto on_error;
		}
	}
	if( libbfio_narrow_string_split(
	     path,
	     path_length + 1,
	     '/',
	     &path_split_string,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_INITIALIZE_FAILED,
		 "%s: unable to split path.",
		 function );

		goto on_error;
	}
	/* Determine the full path size
	 */
	*full_path_size = 0;

	/* If the path is relative
	 * add the size of the current directory
	 * a directory separator, if necessary
	 */
	if( path_type == LIBBFIO_PATH_TYPE_RELATIVE )
	{
		*full_path_size += ( current_directory_size - 1 );

		if( ( current_directory_size >= 2 )
		 && ( current_directory[ current_directory_size - 2 ] != '/' ) )
		{
			*full_path_size += 1;
		}
	}
	if( current_directory_split_string != NULL )
	{
		if( libbfio_narrow_split_string_get_number_of_segments(
		     current_directory_split_string,
		     &current_directory_number_of_segments,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve number of current directory string segments.",
			 function );

			goto on_error;
		}
		current_directory_segment_index = current_directory_number_of_segments - 1;
	}
	if( libbfio_narrow_split_string_get_number_of_segments(
	     path_split_string,
	     &path_number_of_segments,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve number of path string segments.",
		 function );

		goto on_error;
	}
	for( path_segment_index = 0;
	     path_segment_index < path_number_of_segments;
	     path_segment_index++ )
	{
		if( libbfio_narrow_split_string_get_segment_by_index(
		     path_split_string,
		     path_segment_index,
		     &path_string_segment,
		     &path_string_segment_size,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve path string segment: %d.",
			 function,
			 path_segment_index );

			goto on_error;
		}
		if( path_string_segment == NULL )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
			 "%s: missing path string segment: %d.",
			 function,
			 path_segment_index );

			goto on_error;
		}
		/* If the path is .. reverse the current path by one directory
		 */
		if( ( path_string_segment_size == 3 )
		 && ( path_string_segment[ 0 ] == '.' )
		 && ( path_string_segment[ 1 ] == '.' ) )
		{
			if( last_used_path_segment_index == 0 )
			{
				if( libbfio_narrow_split_string_get_segment_by_index(
				     current_directory_split_string,
				     current_directory_segment_index,
				     &current_directory_string_segment,
				     &current_directory_string_segment_size,
				     error ) != 1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_GET_FAILED,
					 "%s: unable to retrieve current directory string segment: %d.",
					 function,
					 current_directory_segment_index );

					goto on_error;
				}
				if( current_directory_string_segment == NULL )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
					 "%s: missing current directory string segment: %d.",
					 function,
					 current_directory_segment_index );

					goto on_error;
				}
				/* Remove the size of the parent directory name and a directory separator
				 * Note that the size includes the end of string character
				 */
				*full_path_size -= current_directory_string_segment_size;

				if( libbfio_narrow_split_string_set_segment_by_index(
				     current_directory_split_string,
				     current_directory_segment_index,
				     NULL,
				     0,
				     error ) != 1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_SET_FAILED,
					 "%s: unable to set current directory string segment: %d.",
					 function,
					 current_directory_segment_index );

					goto on_error;
				}
				current_directory_segment_index--;
			}
			else
			{
				if( libbfio_narrow_split_string_get_segment_by_index(
				     path_split_string,
				     last_used_path_segment_index,
				     &last_used_path_string_segment,
				     &last_used_path_string_segment_size,
				     error ) != 1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_GET_FAILED,
					 "%s: unable to retrieve last used path string segment: %d.",
					 function,
					 last_used_path_segment_index );

					goto on_error;
				}
				if( last_used_path_string_segment == NULL )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
					 "%s: missing last used path string string segment: %d.",
					 function,
					 last_used_path_segment_index );

					goto on_error;
				}
				/* Remove the size of the parent directory name and a directory separator
				 * Note that the size includes the end of string character
				 */
				*full_path_size -= last_used_path_string_segment_size;

				if( libbfio_narrow_split_string_set_segment_by_index(
				     path_split_string,
				     path_segment_index,
				     NULL,
				     0,
				     error ) != 1 )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_SET_FAILED,
					 "%s: unable to set path string segment: %d.",
					 function,
					 path_segment_index );

					goto on_error;
				}
				/* Find the previous path split value that contains a name
				 */
				while( last_used_path_segment_index > 0 )
				{
					last_used_path_segment_index--;

					if( libbfio_narrow_split_string_get_segment_by_index(
					     path_split_string,
					     last_used_path_segment_index,
					     &last_used_path_string_segment,
					     &last_used_path_string_segment_size,
					     error ) != 1 )
					{
						liberror_error_set(
						 error,
						 LIBERROR_ERROR_DOMAIN_RUNTIME,
						 LIBERROR_RUNTIME_ERROR_GET_FAILED,
						 "%s: unable to retrieve last used path string segment: %d.",
						 function,
						 last_used_path_segment_index );

						goto on_error;
					}
					if( last_used_path_string_segment_size != 0 )
					{
						break;
					}
				}
			}
			if( libbfio_narrow_split_string_set_segment_by_index(
			     path_split_string,
			     path_segment_index,
			     NULL,
			     0,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_SET_FAILED,
				 "%s: unable to set path string segment: %d.",
				 function,
				 path_segment_index );

				goto on_error;
			}
		}
		/* If the path is . ignore the entry
		 */
		else if( ( path_string_segment_size == 2 )
		      && ( path_string_segment[ 0 ] == '.' ) )
		{
			if( libbfio_narrow_split_string_set_segment_by_index(
			     path_split_string,
			     path_segment_index,
			     NULL,
			     0,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_SET_FAILED,
				 "%s: unable to set path string segment: %d.",
				 function,
				 path_segment_index );

				goto on_error;
			}
		}
		else
		{
			/* Add the size of the directory or file name and a directory separator
			 * Note that the size includes the end of string character
			 */
			*full_path_size += path_string_segment_size;

			last_used_path_segment_index = path_segment_index;
		}
	}
	/* Note that the last path separator serves as the end of string
	 */
	full_path_index = 0;

	*full_path = libcstring_narrow_string_allocate(
	              *full_path_size );

	if( *full_path == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_MEMORY,
		 LIBERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create full path.",
		 function );

		goto on_error;
	}
	/* If the path is relative
	 * add the current directory elements
	 */
	if( ( path_type == LIBBFIO_PATH_TYPE_RELATIVE )
	 && ( current_directory_split_string != NULL ) )
	{
		for( current_directory_segment_index = 0;
		     current_directory_segment_index < current_directory_number_of_segments;
		     current_directory_segment_index++ )
		{
			if( libbfio_narrow_split_string_get_segment_by_index(
			     current_directory_split_string,
			     current_directory_segment_index,
			     &current_directory_string_segment,
			     &current_directory_string_segment_size,
			     error ) != 1 )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_GET_FAILED,
				 "%s: unable to retrieve current directory string segment: %d.",
				 function,
				 current_directory_segment_index );

				goto on_error;
			}
			if( current_directory_string_segment_size != 0 )
			{
				if( current_directory_string_segment == NULL )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
					 "%s: missing current directory string segment: %d.",
					 function,
					 current_directory_segment_index );

					goto on_error;
				}
				if( libcstring_narrow_string_copy(
				     &( ( *full_path )[ full_path_index ] ),
				     current_directory_string_segment,
				     current_directory_string_segment_size - 1 ) == NULL )
				{
					liberror_error_set(
					 error,
					 LIBERROR_ERROR_DOMAIN_RUNTIME,
					 LIBERROR_RUNTIME_ERROR_SET_FAILED,
					 "%s: unable to set current directory split value: %d in full path.",
					 function,
					 current_directory_segment_index );

					goto on_error;
				}
				full_path_index += current_directory_string_segment_size - 1;

				( *full_path )[ full_path_index ] = '/';

				full_path_index += 1;
			}
		}
	}
	for( path_segment_index = 0;
	     path_segment_index < path_number_of_segments;
	     path_segment_index++ )
	{
		if( libbfio_narrow_split_string_get_segment_by_index(
		     path_split_string,
		     path_segment_index,
		     &path_string_segment,
		     &path_string_segment_size,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve path string segment: %d.",
			 function,
			 path_segment_index );

			goto on_error;
		}
		if( path_string_segment_size != 0 )
		{
			if( path_string_segment == NULL )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_VALUE_MISSING,
				 "%s: missing path string segment: %d.",
				 function,
				 path_segment_index );

				goto on_error;
			}
			if( libcstring_narrow_string_copy(
			     &( ( *full_path )[ full_path_index ] ),
			     path_string_segment,
			     path_string_segment_size - 1 ) == NULL )
			{
				liberror_error_set(
				 error,
				 LIBERROR_ERROR_DOMAIN_RUNTIME,
				 LIBERROR_RUNTIME_ERROR_SET_FAILED,
				 "%s: unable to set path split value: %d in full path.",
				 function,
				 path_segment_index );

				goto on_error;
			}
			full_path_index += path_string_segment_size - 1;

			( *full_path )[ full_path_index ] = '/';

			full_path_index += 1;
		}
	}
	( *full_path )[ full_path_index - 1 ] = 0;

	if( libbfio_narrow_split_string_free(
	     &path_split_string,
	     error ) != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_FINALIZE_FAILED,
		 "%s: unable to free path split string.",
		 function );

		goto on_error;
	}
	if( current_directory_split_string != NULL )
	{
		if( libbfio_narrow_split_string_free(
		     &current_directory_split_string,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_FINALIZE_FAILED,
			 "%s: unable to free current directory split string.",
			 function );

			goto on_error;
		}
	}
	if( current_directory != NULL )
	{
		memory_free(
		 current_directory );
	}
	return( 1 );

on_error:
	if( *full_path != NULL )
	{
		memory_free(
		 *full_path );

		*full_path = NULL;
	}
	*full_path_size = 0;

	if( path_split_string != NULL )
	{
		libbfio_narrow_split_string_free(
		 &path_split_string,
		 NULL );
	}
	if( current_directory_split_string != NULL )
	{
		libbfio_narrow_split_string_free(
		 &current_directory_split_string,
		 NULL );
	}
	if( current_directory != NULL )
	{
		memory_free(
		 current_directory );
	}
	return( -1 );
}

#endif /* defined( WINAPI ) */

#if defined( HAVE_WIDE_CHARACTER_TYPE )

/* Retrieves the current working directory
 * Returns 1 if successful or -1 on error
 */
int libbfio_path_get_current_working_directory_wide(
     wchar_t **current_working_directory,
     size_t *current_working_directory_size,
     liberror_error_t **error )
{
	libcstring_system_character_t error_string[ 128 ];

	static char *function                        = "libbfio_path_get_current_working_directory_wide";

#if defined( WINAPI ) && !defined( USE_CRT_FUNCTIONS )
	DWORD safe_current_working_directory_size    = 0;
	DWORD error_code                             = 0;
#endif
#if !defined( WINAPI )
	char *narrow_current_working_directory       = 0;
	size_t narrow_current_working_directory_size = 0;
#endif

	if( current_working_directory == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid current working directory.",
		 function );

		return( -1 );
	}
	if( *current_working_directory != NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_ALREADY_SET,
		 "%s: invalid current working directory value already set.",
		 function );

		return( -1 );
	}
	if( current_working_directory_size == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_ARGUMENTS,
		 LIBERROR_ARGUMENT_ERROR_INVALID_VALUE,
		 "%s: invalid current working directory size.",
		 function );

		return( -1 );
	}
#if defined( WINAPI ) && !defined( USE_CRT_FUNCTIONS )
	safe_current_working_directory_size = GetCurrentDirectoryW(
	                                       0,
	                                       NULL );

	if( safe_current_working_directory_size == 0 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_GET_FAILED,
		 "%s: unable to retrieve current working directory size.",
		 function );

		goto on_error;
	}
	if( (size_t) safe_current_working_directory_size > (size_t) SSIZE_MAX )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_RUNTIME,
		 LIBERROR_RUNTIME_ERROR_VALUE_OUT_OF_BOUNDS,
		 "%s: current working directory size value out of bounds.",
		 function );

		goto on_error;
	}
	*current_working_directory_size = (size_t) safe_current_working_directory_size;

#elif defined( WINAPI )
	*current_working_directory_size = (size_t) _MAX_PATH;

#else
	narrow_current_working_directory = libcstring_narrow_string_allocate(
	                                    PATH_MAX );

	if( narrow_current_working_directory == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_MEMORY,
		 LIBERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create narrow current working directory.",
		 function );

		goto on_error;
	}
	if( getcwd(
	     narrow_current_working_directory,
	     PATH_MAX ) == NULL )
	{
		if( libbfio_error_string_copy_from_error_number(
		     error_string,
		     128,
		     errno,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve current working directory with error: %" PRIs_LIBCSTRING_SYSTEM "",
			 function,
			 error_string );
		}
		else
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve current working directory.",
			 function );
		}
		goto on_error;
	}
	narrow_current_working_directory_size = 1 + libcstring_narrow_string_length(
	                                             narrow_current_working_directory );

	/* Convert the current working directory to a wide string
	 * if the platform has no wide character open function
	 */
	if( libcstring_narrow_system_string_codepage == 0 )
	{
#if SIZEOF_WCHAR_T == 4
		result = libuna_utf32_string_size_from_utf8(
		          (libuna_utf8_character_t *) narrow_current_working_directory,
		          narrow_current_working_directory_size,
		          current_working_directory_size,
		          error );
#elif SIZEOF_WCHAR_T == 2
		result = libuna_utf16_string_size_from_utf8(
		          (libuna_utf8_character_t *) narrow_current_working_directory,
		          narrow_current_working_directory_size,
		          current_working_directory_size,
		          error );
#else
#error Unsupported size of wchar_t
#endif /* SIZEOF_WCHAR_T */
	}
	else
	{
#if SIZEOF_WCHAR_T == 4
		result = libuna_utf32_size_from_byte_stream(
		          (uint8_t *) narrow_current_working_directory,
		          narrow_current_working_directory_size,
		          libcstring_narrow_system_string_codepage,
		          current_working_directory_size,
		          error );
#elif SIZEOF_WCHAR_T == 2
		result = libuna_utf16_size_from_byte_stream(
		          (uint8_t *) narrow_current_working_directory,
		          narrow_current_working_directory_size,
		          libcstring_narrow_system_string_codepage,
		          current_working_directory_size,
		          error );
#else
#error Unsupported size of wchar_t
#endif /* SIZEOF_WCHAR_T */
	}
	if( result != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_CONVERSION,
		 LIBERROR_CONVERSION_ERROR_GENERIC,
		 "%s: unable to determine wide character current working directory size.",
		 function );

		return( -1 );
	}
#endif

	*current_working_directory = libcstring_wide_string_allocate(
	                              *current_working_directory_size );

	if( *current_working_directory == NULL )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_MEMORY,
		 LIBERROR_MEMORY_ERROR_INSUFFICIENT,
		 "%s: unable to create current working directory.",
		 function );

		goto on_error;
	}
#if defined( WINAPI ) && !defined( USE_CRT_FUNCTIONS )
	if( GetCurrentDirectoryW(
	     safe_current_working_directory_size,
	     *current_working_directory ) != ( safe_current_working_directory_size - 1 ) )
	{
		error_code = GetLastError();

		if( libbfio_error_string_copy_from_error_number(
		     error_string,
		     128,
		     error_code,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve current working directory with error: %" PRIs_LIBCSTRING_SYSTEM "",
			 function,
			 error_string );
		}
		else
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve current working directory.",
			 function );
		}
		goto on_error;
	}
#elif defined( WINAPI )
	if( _wgetcwd(
	     *current_working_directory,
	     *current_working_directory_size ) == NULL )
	{
		if( libbfio_error_string_copy_from_error_number(
		     error_string,
		     128,
		     errno,
		     error ) != 1 )
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve current working directory with error: %" PRIs_LIBCSTRING_SYSTEM "",
			 function,
			 error_string );
		}
		else
		{
			liberror_error_set(
			 error,
			 LIBERROR_ERROR_DOMAIN_RUNTIME,
			 LIBERROR_RUNTIME_ERROR_GET_FAILED,
			 "%s: unable to retrieve current working directory.",
			 function );
		}
		goto on_error;
	}
#else
	if( libcstring_narrow_system_string_codepage == 0 )
	{
#if SIZEOF_WCHAR_T == 4
		result = libuna_utf32_string_copy_from_utf8(
		          (libuna_utf32_character_t *) *current_working_directory,
		          *current_working_directory_size,
		          (libuna_utf8_character_t *) narrow_current_working_directory,
		          narrow_current_working_directory_size,
		          error );
#elif SIZEOF_WCHAR_T == 2
		result = libuna_utf16_string_copy_from_utf8(
		          (libuna_utf16_character_t *) *current_working_directory,
		          *current_working_directory_size,
		          (libuna_utf8_character_t *) narrow_current_working_directory,
		          narrow_current_working_directory_size,
		          error );
#else
#error Unsupported size of wchar_t
#endif /* SIZEOF_WCHAR_T */
	}
	else
	{
#if SIZEOF_WCHAR_T == 4
		result = libuna_utf32_copy_from_byte_stream(
		          (libuna_utf32_character_t *) *current_working_directory,
		          *current_working_directory_size,
		          (uint8_t *) narrow_current_working_directory,
		          narrow_current_working_directory_size,
		          libcstring_narrow_system_string_codepage,
		          error );
#elif SIZEOF_WCHAR_T == 2
		result = libuna_utf16_copy_from_byte_stream(
		          (libuna_utf16_character_t *) *current_working_directory,
		          *current_working_directory_size,
		          (uint8_t *) narrow_current_working_directory,
		          narrow_current_working_directory_size,
		          libcstring_narrow_system_string_codepage,
		          error );
#else
#error Unsupported size of wchar_t
#endif /* SIZEOF_WCHAR_T */
	}
	if( result != 1 )
	{
		liberror_error_set(
		 error,
		 LIBERROR_ERROR_DOMAIN_CONVERSION,
		 LIBERROR_CONVERSION_ERROR_GENERIC,
		 "%s: unable to set current working directory.",
		 function );

		goto on_error;
	}
#endif
	return( 1 );

on_error:
	if( current_working_directory != NULL )
	{
		memory_free(
		 *current_working_directory );

		*current_working_directory = NULL;
	}
	*current_working_directory_size = 0;

#if !defined( WINAPI )
	if( narrow_current_working_directory != NULL )
	{
		memory_free(
		 narrow_current_working_directory );
	}
#endif
	return( -1 );
}

#endif /* defined( HAVE_WIDE_CHARACTER_TYPE ) */

