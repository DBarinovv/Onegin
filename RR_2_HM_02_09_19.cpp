//=============================================================================
// SORTING TEXT IN LEXICOGRAPHICAL ORDER
//=============================================================================

#include "TXLib.h"
#include <stdio.h>
#include <sys\stat.h>
#include <assert.h>

//=============================================================================

void make_array_of_pointers (int *ln_Strings, char **right_arr, char **strings,
                             char *buf, int n_Lines, int sz_File);

char *read_file (int *n_Lines, int *sz_File, FILE **file_out_pointer);

//-----------------------------------------------------------------------------

int compare_1 (const char *str_1, const char *str_2);

int compare_2 (const char *str_1, const char *str_2, int len_1, int len_2);

//-----------------------------------------------------------------------------

void quick_sort (int *ln_Strings, char **strings, int left, int right, int direction);

void my_swap (char **elem_1, char **elem_2, int *len_1, int *len_2);

void sort_for_two_elem (char **elem_1, char **elem_2, int *len_1, int *len_2, int direction);

int sort_for_part_1 (int *ln_Strings, char **strings, int left, int right, int mid);

int sort_for_part_2 (int *ln_Strings, char **strings, int left, int right, int mid);

//-----------------------------------------------------------------------------

void output (char **strings, int n_Lines);

void f_output (char **strings, int n_Lines, FILE *file_out_pointer);

//=============================================================================

int main ()
{
    int direction = 1;  // 1 if we want to sort from left to right, other, if right to left

    int n_Lines = 1;    // number of lines
    int sz_File = 0;    // size of file
    FILE *file_out_pointer = 0;

    char *buf = read_file (&n_Lines, &sz_File, &file_out_pointer);

    assert (file_out_pointer);
    assert (n_Lines >= 0);

    char *strings[n_Lines];       // array with pointers to strings
    char *right_arr[n_Lines];     // array with pointers (to display original text)
    int ln_Strings[n_Lines];      // array of strings lens

    make_array_of_pointers (ln_Strings, right_arr, strings, buf, n_Lines, sz_File);

    quick_sort (ln_Strings, strings, 0, n_Lines - 1, direction);

//    printf ("\nSorted from left to right:\n");
    fprintf (file_out_pointer, "\nSorted from left to right:\n");
//    output (strings, n_Lines);                     // output right (sorted) array
    f_output (strings, n_Lines, file_out_pointer);   // output right (sorted) array in file

    direction = -1;  // change direction to right to left

    quick_sort (ln_Strings, strings, 0, n_Lines - 1, direction);

//    printf ("\nSorted from right to left:\n");
    fprintf (file_out_pointer, "\nSorted from right to left:\n");
//    output (strings, n_Lines);                     // output sorted right to left array in file
    f_output (strings, n_Lines, file_out_pointer);   // output sorted right to left array in file

//    printf ("\nOriginal:\n");
    fprintf (file_out_pointer, "\nOriginal text:\n");
//    output (right_arr, n_Lines);                   // output original text
    f_output (right_arr, n_Lines, file_out_pointer); // output original text in file

    return 0;
}

//{============================================================================
//! Make array (strings),
//! in which elem [i] is pointer to the start of string number [i] in the text
//}============================================================================

void make_array_of_pointers (int *ln_Strings, char **right_arr, char **strings,
                             char *buf, int n_Lines, int sz_File)
{
    strings[0] = buf;
    right_arr[0] = buf;

    int pos  = 1;
    int line = 0;

    while (pos < sz_File - 1 && line < n_Lines - 1)
    {
        if (buf[pos] == '\0')
        {
            line++;
            assert(0 <= line && line < n_Lines);
            strings[line] = (buf + pos + 1);
            right_arr[line] = (buf + pos + 1);
            ln_Strings[line - 1] = strings[line] - strings[line - 1];
        }
        pos++;
    }
    ln_Strings[line] = buf + pos - strings[line];
}

//{============================================================================
//! Read file and make buffer for text
//}============================================================================

char *read_file (int *n_Lines, int *sz_File, FILE **file_out_pointer)
{
    struct stat text = {};

    FILE *file_pointer = fopen ("Onegin.txt", "r");
    *file_out_pointer = fopen ("Onegin_out.txt", "w");

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

int compare_1 (const char *str_1, const char *str_2)
{
    while (*str_1 == *str_2 && *str_1 != '\0' && *str_2 != '\0')
    {
        str_1++;
        str_2++;
    }

    if (*str_1 == '\0' && *str_2 == '\0')
        return 0;
    else if (*str_1 == '\0')
        return -1;
    else if (*str_2 == '\0')
        return 1;

    return *str_1 - *str_2;
}

//{============================================================================
//! Compare 2 lines from their end (except punctuation) and output
//! 0 if str_1 = str_2
//! >0 if str_1 > str_2
//! <0 if str_1 < str_2
//}============================================================================

int compare_2 (const char *str_1, const char *str_2, int len_1, int len_2)
{
    if (len_1 > 0)
        len_1--;
    if (len_2 > 0)
        len_2--;

    assert (len_1 >= 0);
    assert (len_2 >= 0);

//-----------------------------------------------------------------------------
// skip punctuation
//-----------------------------------------------------------------------------

    int ok_1 = 0, ok_2 = 0;

    while (len_1 >= 0 && len_2 >= 0 && (ok_1 == 0 || ok_2 == 0))
        {
            if (('a' <= *(str_1 + len_1) && *(str_1 + len_1) <= 'z') ||
                ('A' <= *(str_1 + len_1) && *(str_1 + len_1) <= 'Z'))
            {
                ok_1 = 1;
            }
            else
                len_1--;

            if (('a' <= *(str_2 + len_2) && *(str_2 + len_2) <= 'z') ||
                ('A' <= *(str_2 + len_2) && *(str_2 + len_2) <= 'Z'))
            {
                ok_2 = 1;
            }
            else
                len_2--;
        }

//-----------------------------------------------------------------------------

    while (len_1 >= 0 && len_2 >= 0 && (*(str_1 + len_1) == *(str_2 + len_2)))
    {
        len_1--;
        len_2--;
    }

    if (len_1 < 0 && len_2 < 0)
        return 0;             // equal
    else if (len_1 < 0)
        return -1;            // str_1 < str_2
    else if (len_2 < 0)
        return 1;             // str_1 > str_2
    else
        return *(str_1 + len_1) - *(str_2 + len_2);
}

//============================================================================

void quick_sort (int *ln_Strings, char **strings, int left, int right, int direction)
{
    if (right - left + 1 == 2)
    {
        assert (left >= 0 && right >= 0);
        sort_for_two_elem (&strings[left], &strings[right], &ln_Strings[left], &ln_Strings[right], direction);
    }
    else if (right - left + 1 == 1 || right - left + 1 == 0)
    {
        return ;
    }
    else
    {
        int mid = -1;
        if (direction == 1)
        {
            mid = sort_for_part_1 (ln_Strings, strings, left, right, ((left + right) / 2));
        }
        else
        {
            mid = sort_for_part_2 (ln_Strings, strings, left, right, ((left + right) / 2));
        }

        quick_sort (ln_Strings, strings, left, mid - 1, direction);
        quick_sort (ln_Strings, strings, mid + 1, right, direction);
    }
}

//{============================================================================
//! Swaps two elements
//}============================================================================

void my_swap (char **elem_1, char **elem_2, int *len_1, int *len_2)
{
    char* helper_1 = *elem_1;
    *elem_1 = *elem_2;
    *elem_2 = helper_1;

    int helper_2 = *len_1;
    *len_1 = *len_2;
    *len_2 = helper_2;
}

//{============================================================================
//! Case of 2 elements
//}============================================================================

void sort_for_two_elem (char **elem_1, char **elem_2, int *len_1, int *len_2, int direction)
{
    if (direction == 1)
    {
        if (compare_1 (*elem_1, *elem_2) > 0)
        {
            my_swap (elem_1, elem_2, len_1, len_2);
        }
    }
    else
    {
        if (compare_2 (*elem_1, *elem_2, *len_1, *len_2) > 0)
        {
            my_swap (elem_1, elem_2, len_1, len_2);
        }
    }
}

//{============================================================================
//! Make array right form:
//! All elements < mid located on the left
//! All elements > mid located on the right
//}============================================================================

int sort_for_part_1 (int *ln_Strings, char **strings, int left, int right, int mid)
{
    int res = mid;

    while (left < right)
    {
        assert(left >= 0 && right >= 0 && res >= 0);
        while (compare_1 (strings[left], strings[res]) < 0 && left < right)
        {
            left++;
        }

        while (compare_1 (strings[right], strings[res]) > 0 && left < right)
        {
            right--;
        }

        if (left < right)
        {
            if (compare_1 (strings[left], strings[res]) == 0)
                res = right;
            else if (compare_1 (strings[right], strings[res]) == 0)
                res = left;
            my_swap (&strings[left], &strings[right], &ln_Strings[left], &ln_Strings[right]);
        }
    }
    return res;
}

//{============================================================================
//! Make array right form (but for direction = -1):
//! All elements < mid located on the left
//! All elements > mid located on the right
//}============================================================================

int sort_for_part_2 (int *ln_Strings, char **strings, int left, int right, int mid)
{
    int res = mid;
    assert (res >= 0);

    while (left < right)
    {
        assert(left >= 0 && right >= 0 && res >= 0);
        while (compare_2 (strings[left], strings[res], ln_Strings[left], ln_Strings[right]) < 0 && left < right)
        {
            left++;
        }

        while (compare_2 (strings[right], strings[res], ln_Strings[left], ln_Strings[right]) > 0 && left < right)
        {
            right--;
        }

        if (left < right)
        {
            if (compare_2 (strings[left], strings[res], ln_Strings[left], ln_Strings[right]) == 0)
                res = right;
            else if (compare_2 (strings[right], strings[res], ln_Strings[left], ln_Strings[right]) == 0)
                res = left;

            my_swap (&strings[left], &strings[right], &ln_Strings[left], &ln_Strings[right]);
        }
    }
    return res;
}

//{============================================================================
//! Display array in right order
//}============================================================================

void output (char **strings, int n_Lines)
{
    for (int i = 0; i < n_Lines; i++)
    {
        printf ("[%d]: (%s)\n", i, strings[i]);
    }
}

//{============================================================================
//! Output in file
//}============================================================================

void f_output (char **strings, int n_Lines, FILE *file_out_pointer)
{
    assert (file_out_pointer != 0);

    for (int i = 0; i < n_Lines; i++)
    {
        fprintf (file_out_pointer, "[%d]: (%s)\n", i, strings[i]);
    }
}
