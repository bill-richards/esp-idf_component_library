#include <stdio.h>
#include <string.h>
#include <ctype.h>

#include "gsdc_string_utils.h"

/* Converts a hex character to its integer value */
char from_hex(char ch) {
  return isdigit(ch) ? ch - '0' : tolower(ch) - 'a' + 10;
}

/* Converts an integer value to its hex character*/
char to_hex(char code) {
  static char hex[] = "0123456789abcdef";
  return hex[code & 15];
}

char * gsdc_string_utils_url_encode(char * str) {
  char *pstr = str, *buf = malloc(strlen(str) * 3 + 1), *pbuf = buf;
  while (*pstr) {
    if (isalnum(*pstr) || *pstr == '-' || *pstr == '_' || *pstr == '.' || *pstr == '~') 
      *pbuf++ = *pstr;
    else if (*pstr == ' ') 
      *pbuf++ = '+';
    else 
      *pbuf++ = '%', *pbuf++ = to_hex(*pstr >> 4), *pbuf++ = to_hex(*pstr & 15);
    pstr++;
  }
  *pbuf = '\0';
  return buf;
}

char * gsdc_string_utils_url_decode(char * str) {
  char *pstr = str, *buf = malloc(strlen(str) + 1), *pbuf = buf;
  while (*pstr) {
    if (*pstr == '%') {
      if (pstr[1] && pstr[2]) {
        *pbuf++ = from_hex(pstr[1]) << 4 | from_hex(pstr[2]);
        pstr += 2;
      }
    } else if (*pstr == '+') { 
      *pbuf++ = ' ';
    } else {
      *pbuf++ = *pstr;
    }
    pstr++;
  }
  *pbuf = '\0';
  return buf;
}

// Function to replace all the occurrences
// of the substring S1 to S2 in string S
char * gsdc_string_utils_replace_substring(char* input_string, char* to_be_replaced, char* replace_with)
{
	// Stores the resultant string
	// char ans[1000] = { 0 };
  	char * ans = (char *)calloc(strlen(input_string)*2, sizeof(char));
	int ans_idx = 0;

	// Traverse the string input_string
	for (int i = 0; i < strlen(input_string); i++) {

		int k = 0;

		// If the first character of
		// string to_be_replaced matches with the
		// current character in string input_string
		if (input_string[i] == to_be_replaced[k] && i + strlen(to_be_replaced) <= strlen(input_string)) {

			int j;

			// If the complete string
			// matches or not
			for (j = i; j < i + strlen(to_be_replaced); j++) {

				if (input_string[j] != to_be_replaced[k]) {
					break;
				}
				else {
					k = k + 1;
				}
			}

			// If complete string matches
			// then replace it with the
			// string replace_with
			if (j == i + strlen(to_be_replaced)) {
				for (int l = 0; l < strlen(replace_with); l++) {
					ans[ans_idx++] = replace_with[l];
				}
				i = j - 1;
			}

			// Otherwise
			else {
				ans[ans_idx++] = input_string[i];
			}
		}

		// Otherwise
		else {
			ans[ans_idx++] = input_string[i];
		}
	}

	return ans;
}