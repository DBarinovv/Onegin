//=============================================================================
// SORTING TEXT IN LEXICOGRAPHICAL ORDER
//=============================================================================

#include "TXLib.h"
#include <stdio.h>
#include <sys\stat.h>
#include <stdlib.h>
#include <assert.h>

//-----------------------------------------------------------------------------

char *read_file (int *n_Lines, int *sz_File);

int compare (const char *str_1, const char *str_2);

void quick_sort (char **strings, int left, int right);

void my_swap (char **elem_1, char **elem_2);

void sort_for_two_elem (char **elem_1, char **elem_2);

int sort_for_part (char **strings, int left, int right, int mid);

void output (char **strings, int n_Lines);

//-----------------------------------------------------------------------------

int main ()
{
    int n_Lines = 1;    // number of lines
    int sz_File = 0;    // size of file

    char *buf = read_file (&n_Lines, &sz_File);

//-----------------------------------------------------------------------------
// Make array (strings),
// in which elem [i] is pointer to the start of string number [i] in the text
//-----------------------------------------------------------------------------

    assert (n_Lines >= 0);
    char *strings[n_Lines];

    strings[0] = buf;

    int pos  = 1;
    int line = 0;

    while (pos < sz_File - 1 && line < n_Lines - 1)
    {
        if (buf[pos] == '\0')
        {
            line++;
            assert(0 <= line && line < n_Lines);
            strings[line] = (buf + pos + 1);
        }
        pos++;
    }

//-----------------------------------------------------------------------------

    quick_sort (strings, 0, n_Lines - 1);

    // output right (sorted) array
    output (strings, n_Lines);

    return 0;
}

//{============================================================================
//! Make array of pointers
//}============================================================================

char *read_file (int *n_Lines, int *sz_File)
{
    struct stat text = {};

    FILE *file_pointer = fopen ("Onegin.txt", "r");

    stat ("Onegin.txt", &text);
    *sz_File = text.st_size;

    char *buf = (char *)calloc (*sz_File + 1, sizeof(char)); // make buffer for text

    fread (buf, sizeof(char), *sz_File, file_pointer);

//-----------------------------------------------------------------------------
// find number of lines
//-----------------------------------------------------------------------------

    for (int k = 0; k < *sz_File - 1; k++)
    {
        if (*(buf + k) == '\n')
        {
            (*n_Lines)++;
            buf[k] = '\0';
        }
    }

//-----------------------------------------------------------------------------

    fclose (file_pointer);

    return buf;
}

//{============================================================================
//! Compare 2 lines and output
//! 0 if str_1 = str_2
//! >0 if str_1 > str_2
//! <0 if str_1 < str_2
//}============================================================================

int compare (const char *str_1, const char *str_2)
{
    for (; *str_1 == *str_2; str_1++, str_2++)
    {
        if (*str_1 == '\0')
            return 0;
    }

    return *str_1 - *str_2;
}

//{============================================================================

void quick_sort (char **strings, int left, int right)
{
    if (right - left + 1 == 2)
    {
        assert(left >= 0 && right >= 0);
        sort_for_two_elem (&strings[left], &strings[right]);
    }
    else if (right - left + 1 == 1 || right - left + 1 == 0)
    {
        return ;
    }
    else
    {
        int mid = sort_for_part (strings, left, right, ((left + right) / 2));

        quick_sort (strings, left, mid - 1);
        quick_sort (strings, mid + 1, right);
    }
}

//{============================================================================
//! Swaps two elements
//}============================================================================

void my_swap (char **elem_1, char **elem_2)
{
    char* helper = *elem_1;
    *elem_1 = *elem_2;
    *elem_2 = helper;
}

//{============================================================================
//! Case of 2 elements
//}============================================================================

void sort_for_two_elem (char **elem_1, char **elem_2)
{
    if (compare (*elem_1, *elem_2) > 0)
    {
        my_swap (elem_1, elem_2);
    }
}

//{============================================================================
//! Make array right form:
//! All elements < mid located on the left
//! All elements > mid located on the right
//}============================================================================

int sort_for_part (char **strings, int left, int right, int mid)
{
    int res = mid;

    while (left < right)
    {
        assert(left >= 0 && right >= 0 && res >= 0);
        while (compare (strings[left], strings[res]) < 0 && left < right)
        {
            left++;
        }

        while (compare (strings[right], strings[res]) > 0 && left < right)
        {
            right--;
        }

        if (left < right)
        {
            if (compare (strings[left], strings[res]) == 0)
                res = right;
            else if (compare (strings[right], strings[res]) == 0)
                res = left;

            my_swap (&strings[left], &strings[right]);
        }
    }
    return res;
}


//{============================================================================
//! Print array in right order
//}============================================================================

void output (char **strings, int n_Lines)
{
    for (int i = 0; i < n_Lines; i++)
    {
        printf ("[%d]: (%s)\n", i, strings[i]);
    }
}
