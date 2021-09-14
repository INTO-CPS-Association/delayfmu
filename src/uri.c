/*
 * uri.c
 *
 *  Created on: Sep 8, 2016
 *      Author: kel
 *
 *      The source is copied from:
 * https://github.com/controllab/fmi-export-20sim/blob/master/StandaloneFMU/src/%25FMI_PREFIX%25Functions.c
 *      with SHAR1: d6bc6156bee315c802606db5db02c29582eee7b2
 */

#include "uri.h"

#if defined WIN32 || defined WIN64
static const char native_path_separator = '\\';
static const char foreign_path_separator = '/';
#else
static const char native_path_separator = '/';
static const char foreign_path_separator = '\\';
#endif

/**
 * Convert an uri provided by the the co-simulator to a native path
 * @param uri Input path. native and file:/ and file:/// uri's right now
 * @return Natve path that ends with a path separator.
 * Note that the caller is responsible for free-ing the allocated string buffer
 * memory
 */
const char* URIToNativePath(const char* uri) {
  unsigned int path_start = 0;
  char* path = NULL;
  unsigned int path_len = 0;
  unsigned int uri_len = 0;
  char* pch = NULL;
  unsigned int i = 0;
  unsigned int j = 0;
  char buf[3] = "00";

  if (!uri) {
    return NULL;
  }

  uri_len = (unsigned int)strlen(uri);

  if (uri_len == 0) {
    return NULL;
  }

  /* Check if we got a file:/// uri */
  if (strncmp(uri, "file:///", 8) == 0) {
    if (uri[9] == ':') {
      // Windows drive letter in the URI (e.g. file:///c:/ uri
      /* Remove the file:/// */
      path_start = 8;
    } else {
      /* Remove the file:// but keep the third / */
      path_start = 7;
    }
  }
#if defined WIN32 || defined WIN64
  /* Check if we got a file://hostname/path uri */
  else if (strncmp(uri, "file://", 7) == 0) {
    /* Convert to a network share path: //hostname/path */
    path_start = 5;
  }
#endif
  /* Check if we got a file:/ uri */
  else if (strncmp(uri, "file:/", 6) == 0) {
    if (uri[7] == ':') {
      // Windows drive letter in the URI (e.g. file:/c:/ uri
      /* Remove the  file:/ */
      path_start = 6;
    } else {
      /* Remove the file: but keep the / */
      path_start = 5;
    }
  }
  /* Assume that it is a native path */
  else {
    path_start = 0;
  }

  /* Check the length of the remaining string */
  path_len = (int)strlen(&uri[path_start]);
  if (path_len == 0) {
    return NULL;
  }

  /* Allocate memory for the return value including terminating \0 and extra
   * path separator */
  //%IF%%FMI1%
  //	if (g_fmiCallbackFunctions.allocateMemory != NULL)
  //	{
  //		path = (char*) g_fmiCallbackFunctions.allocateMemory(path_len +
  // 2,
  // sizeof(char));
  //	}
  //%ENDIF%
  //%IF%%FMI2%
  //	if ((g_fmiCallbackFunctions) &&( g_fmiCallbackFunctions->allocateMemory
  //!= NULL))
  //	{
  //		path = (char*) g_fmiCallbackFunctions->allocateMemory(path_len +
  // 2,
  // sizeof(char));
  //	}
  //%ENDIF%
  //	else
  //	{
  path = (char*)malloc(path_len + 2);
  //	}

  /* Copy the remainder of the uri and replace all percent encoded character
  * by their ASCII character and translate slashes to backslashes on Windows
  * and backslashes to slashes on other OSses
  */
  for (i = path_start, j = 0; i < uri_len; i++, j++) {
    if (uri[i] == '%') {
      /* Replace the precent-encoded hexadecimal digits by its US-ASCII
      * representation */
      if (i < uri_len - 2) {
        if ((isxdigit(uri[i + 1])) && (isxdigit(uri[i + 2]))) {
          strncpy(buf, uri + i + 1, 2);
          path[j] = (unsigned char)strtol(buf, NULL, 16);
          i += 2;
          path_len -= 2;
        } else {
          /* Not percent encoded, keep the % */
          path[j] = uri[i];
        }
      } else {
        /* Not percent encoded, keep the % */
        path[j] = uri[i];
      }
    } else if (uri[i] == foreign_path_separator) {
      /* Translate slashes to backslashes on Windows and backslashes to slashes
       * on other OSses */
      path[j] = native_path_separator;
    } else {
      /* Just copy the character */
      path[j] = uri[i];
    }
  }

  /* Check if we need to add a path separator at the end */
  if (path[path_len - 1] == native_path_separator) {
    path[path_len] = '\0';
  } else {
    path[path_len] = native_path_separator;
  }
  /* Make sure that the string is always NULL terminated */
  path[path_len + 1] = '\0';

  return path;
}
