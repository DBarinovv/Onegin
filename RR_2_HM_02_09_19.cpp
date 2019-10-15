//=============================================================================
// SORTING TEXT IN LEXICOGRAPHICAL ORDER
//=============================================================================

#include "TXLib.h"
#include <stdio.h>
#include <sys\stat.h>
#include <assert.h>
#include <stdlib.h>

//=============================================================================

struct string_t
{
    char *str;
    int len;
};

struct file_t
{
    struct string_t* strings;
    char* *copies;
    int n_Lines;
    int sz_File;
    FILE *fout;
};

//=============================================================================

struct file_t read_file_and_make_arrays (char *fin, char *fout);

void make_array_of_structs (char **strings_copy, struct string_t *strings_arr,
                            char **buf, int n_Lines, int sz_File);

int find_sz_file (char *fin);

//-----------------------------------------------------------------------------

int compare_1 (const char *str_1, const char *str_2);

int compare_2 (char *str_1, char *str_2);

//-----------------------------------------------------------------------------

void quick_sort_forward (struct string_t *strings_arr, int left, int right);

void quick_sort_back (struct string_t *strings_arr, int left, int right);

void my_swap (char **elem_1, char **elem_2, int *len_1, int *len_2);

void sort_for_two_elem_1 (char **elem_1, char **elem_2, int *len_1, int *len_2);

void sort_for_two_elem_2 (char **elem_1, char **elem_2, int *len_1, int *len_2);

int sort_for_part_1 (struct string_t *strings_arr, int left, int right, int mid);

int sort_for_part_2 (struct string_t *strings_arr, int left, int right, int mid);

//-----------------------------------------------------------------------------

void output_st (struct string_t *strings_arr, int n_Lines);

void output (char* *strings_copy, int n_Lines);

void f_output_st (struct string_t *strings_arr, int n_Lines, FILE *file_out);

void f_output (char* *strings_copy, int n_Lines, FILE *file_out);

void global_output_1 (struct string_t *strings_arr, char* *strings_copy, int n_Lines, FILE *file_out);

void global_output_2 (struct string_t *strings_arr, char* *strings_copy, int n_Lines, FILE *file_out);

//-----------------------------------------------------------------------------

int if_letter (const char sym);

int number_of_lines (const int sz_File, const char *buf);

void skip_punctuation (char **str_1, char **str_2);

char* *my_free (char **arr);

//=============================================================================

void TESTS_quick_sort_back ();

//=============================================================================

int main ()
{
    printf ("If you want tests, type T, else another:\n");

    char c = 'a';
    if ((c = getchar()) == 'T')
    {
        TESTS_quick_sort_back ();
        return 0;
    }

    char fin[]  = "Onegin.txt";
    char fout[] = "Onegin_out.txt";
    char **strings_copy = nullptr;

    struct file_t text = read_file_and_make_arrays (fin, fout);

    struct string_t *strings_arr = text.strings;
    strings_copy = text.copies;  assert (strings_copy);
    int n_Lines = text.n_Lines;  assert (n_Lines >= 1);
    int sz_File = text.sz_File;  assert (sz_File >= 0);
    FILE *file_out = text.fout;  assert (file_out);

    quick_sort_forward (strings_arr, 0, n_Lines - 1);
    global_output_1 (strings_arr, strings_copy, n_Lines, file_out);

    quick_sort_back (strings_arr, 0, n_Lines - 1);
    global_output_2 (strings_arr, strings_copy, n_Lines, file_out);

    assert (strings_arr);
    assert (strings_copy);

    free (&strings_arr);
    strings_arr = nullptr;
    strings_copy = my_free (strings_copy);

    fclose (file_out);

    return 0;
}

//=============================================================================

struct file_t read_file_and_make_arrays (char *fin, char *fout)
{
    int sz_File = find_sz_file (fin);

    FILE *file     = fopen (fin,  "r");  assert (file);
    FILE *file_out = fopen (fout, "w");  assert (file_out);

    char *buf = (char *) calloc (sz_File + 2, sizeof(char));  assert (buf);   // make buffer for text
    char *h_buf = buf + 1;                                    assert (h_buf);

    fread (h_buf, sizeof (char), sz_File, file);

    int n_Lines = number_of_lines (sz_File, h_buf) + 1;  assert (n_Lines >= 0);

    struct string_t *strings_arr = (string_t *) calloc (n_Lines, sizeof (*strings_arr));  assert (strings_arr);     // array with pointers to string

    char* *strings_copy = (char **) calloc (n_Lines, sizeof (*strings_copy));  assert (strings_copy);     // array with pointers (to display original text)

    make_array_of_structs (strings_copy, strings_arr, &h_buf, n_Lines, sz_File);

    fclose (file);

    struct file_t res;
    res.strings = strings_arr;
    res.copies  = strings_copy;
    res.n_Lines = n_Lines;
    res.sz_File = sz_File;
    res.fout    = file_out;

    return res;
}

//{============================================================================
//! Make array (strings),
//! in which elem [i] is pointer to the start of string number [i] in the text
//}============================================================================

void make_array_of_structs (char* *strings_copy, struct string_t *strings_arr,
                            char* *buf, int n_Lines, int sz_File)
{
    int pos  = -1;
    int line = -1;

    while (pos < sz_File - 1 && line < n_Lines - 1)
    {
        if ((*buf)[pos] == '\0' || (*buf)[pos] == '\n')
        {
            (*buf)[pos] = '\0';

            line++;
            assert(0 <= line && line < n_Lines);

            strings_arr[line].str = (*buf + pos + 1);  assert (*buf + pos + 1);
            strings_copy[line]    = (*buf + pos + 1);

            if (strings_arr[line].str - strings_arr[line - 1].str == 1 ||
                strings_arr[line].str - strings_arr[line - 1].str == 0)
            {
                strings_arr[line - 1].len = 0;
            }
            else
                strings_arr[line - 1].len = strings_arr[line].str - strings_arr[line - 1].str - 2;
        }

        pos++;
    }

    if (*buf + pos - strings_arr[line].str == 1 ||
        *buf + pos - strings_arr[line].str == 0)
    {
        strings_arr[line].len = 0;
    }
    else
       strings_arr[line].len = *buf + pos - strings_arr[line].str - 1;
}

//{============================================================================
//! Read file and make buffer for text
//}============================================================================

int find_sz_file (char *fin)
{
    struct stat text = {};

    stat (fin, &text);

    return text.st_size;
}

//{============================================================================
//! Compare 2 lines and output
//! 0 if str_1 = str_2
//! >0 if str_1 > str_2
//! <0 if str_1 < str_2
//}============================================================================

int compare_1 (const char *str_1, const char *str_2)
{
    for (; *str_1 == *str_2; str_1++, str_2++)
    {
        if (*str_1 == '\0')
            return 0;
    }

    return *str_1 - *str_2;
}

//{============================================================================
//! Compare 2 lines from their end (except punctuation) and output
//! 0 if str_1 = str_2
//! >0 if str_1 > str_2
//! <0 if str_1 < str_2
//}============================================================================

int compare_2 (char *str_1, char *str_2)
{
    assert (str_1 && str_2);

    skip_punctuation (&str_1, &str_2);

    if (*str_1 == '\0' && *str_2 != '\0')
    {
        return -1;
    }

    if (*str_1 != '\0' && *str_2 == '\0')
    {
        return 1;
    }

    for (; *str_1 == *str_2; str_1--, str_2--)
    {
        assert (str_1 && str_2);

        if (*str_1 == '\0')
        {
            return 0;
        }
    }

    return *str_1 - *str_2;
}

//============================================================================

void quick_sort_forward (struct string_t *strings_arr, int left, int right)
{
    if (right - left + 1 == 2)
    {
        assert (left >= 0 && right >= 0);

        sort_for_two_elem_1 (&(strings_arr[left].str), &(strings_arr[right].str),
                             &(strings_arr[left].len), &(strings_arr[right].len));
    }
    else if (right - left + 1 == 1 || right - left + 1 == 0)
    {
        return ;
    }
    else
    {
        int mid = sort_for_part_1 (strings_arr, left, right, ((left + right) / 2));

        quick_sort_forward (strings_arr, left, mid - 1);
        quick_sort_forward (strings_arr, mid + 1, right);
    }
}

//=============================================================================

void quick_sort_back (struct string_t *strings_arr, int left, int right)
{
    if (right - left + 1 == 2)
    {
        assert (left >= 0 && right >= 0);

        sort_for_two_elem_2 (&strings_arr[left].str, &strings_arr[right].str,
                             &strings_arr[left].len, &strings_arr[right].len);
    }
    else if (right - left + 1 == 1 || right - left + 1 == 0)
    {
        return ;
    }
    else
    {
        int mid = sort_for_part_2 (strings_arr, left, right, ((left + right) / 2));

        quick_sort_back (strings_arr, left, mid - 1);
        quick_sort_back (strings_arr, mid + 1, right);
    }
}

//{============================================================================
//! Swaps two elements
//}============================================================================

void my_swap (char **elem_1, char **elem_2, int *len_1, int *len_2)
{
    assert (elem_1 && *elem_1 && elem_2 && *elem_2);

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

void sort_for_two_elem_1 (char **elem_1, char **elem_2, int *len_1, int *len_2)
{
    if (compare_1 (*elem_1, *elem_2) > 0)
    {
        my_swap (elem_1, elem_2, len_1, len_2);
    }
}

//=============================================================================

void sort_for_two_elem_2 (char **elem_1, char **elem_2, int *len_1, int *len_2)
{
    if (compare_2 (*elem_1, *elem_2) > 0)
    {
        my_swap (elem_1, elem_2, len_1, len_2);
    }
}

//{============================================================================
//! Make array right form:
//! All elements < mid located on the left
//! All elements > mid located on the right
//}============================================================================

int sort_for_part_1 (struct string_t *strings_arr, int left, int right, int mid)
{
    int res = mid;

    while (left < right)
    {
        assert (left >= 0 && right >= 0 && res >= 0);

        while (compare_1 (strings_arr[left].str, strings_arr[res].str) < 0 && left < right)
        {
            left++;
        }

        while (compare_1 (strings_arr[right].str, strings_arr[res].str) > 0 && left < right)
        {
            right--;
        }

        if (left < right)
        {
            if (compare_1 (strings_arr[right].str, strings_arr[left].str) == 0)
            {
                if (right == res)
                    left++;
                else if (left == res)
                    right --;
                else
                {
                    left++;
                    right--;
                }
            }
            else
            {
                if (compare_1 (strings_arr[left].str, strings_arr[res].str) == 0)
                    res = right;
                else if (compare_1 (strings_arr[right].str, strings_arr[res].str) == 0)
                    res = left;

                my_swap (&strings_arr[left].str, &strings_arr[right].str,
                         &strings_arr[left].len, &strings_arr[right].len);
            }
        }
    }

    return res;
}

//{============================================================================
//! Make array right form (but for direction = -1):
//! All elements < mid located on the left
//! All elements > mid located on the right
//}============================================================================

int sort_for_part_2 (struct string_t *strings_arr, int left, int right, int mid)
{
    int res = mid;  assert (res >= 0);

    while (left < right)
    {
        assert(left >= 0 && right >= 0 && res >= 0);

        while (compare_2 (strings_arr[left].str + strings_arr[left].len,
                          strings_arr[res].str  + strings_arr[res].len) < 0 && left < right)
        {
            left++;
        }

        while (compare_2 (strings_arr[right].str + strings_arr[right].len,
                          strings_arr[res].str   + strings_arr[res].len) > 0 && left < right)
        {
            right--;
        }

        if (left < right)
        {
            if (compare_2 (strings_arr[left].str  + strings_arr[left].len,
                           strings_arr[right].str + strings_arr[right].len) == 0)
            {
                if (right == res)
                    left++;
                else if (left == res)
                    right --;
                else
                {
                    left++;
                    right--;
                }
            }
            else
            {
                if (compare_2 (strings_arr[left].str + strings_arr[left].len,
                               strings_arr[res].str  + strings_arr[res].len) == 0)
                    res = right;
                else if (compare_2 (strings_arr[right].str + strings_arr[right].len,
                                    strings_arr[res].str   + strings_arr[res].len) == 0)
                    res = left;

                my_swap (&strings_arr[left].str, &strings_arr[right].str,
                         &strings_arr[left].len, &strings_arr[right].len);
            }
        }
    }

    return res;
}

//{============================================================================
//! Display array in right order
//}============================================================================

void output_st (struct string_t *strings_arr, int n_Lines)
{
    for (int i = 0; i < n_Lines; i++)
    {
        printf (" [%d]: (%s) \n", i, strings_arr[i].str);
    }
}

void output (char* *true_arr, int n_Lines)
{
    for (int i = 0; i < n_Lines; i++)
    {
        printf (" [%d]: (%s) \n", i, true_arr[i]);
    }
}

//{============================================================================
//! Output in file
//}============================================================================

void f_output_st (struct string_t *strings_arr, int n_Lines, FILE *file_out)
{
    assert (file_out);

    for (int i = 0; i < n_Lines; i++)
    {
        fprintf (file_out, " [%d]: (%s) \n", i, strings_arr[i].str);
    }
}

void f_output (char* *strings_copy, int n_Lines, FILE *file_out)
{
    assert (file_out);

    for (int i = 0; i < n_Lines; i++)
    {
        fprintf (file_out, " [%d]: (%s) \n", i, strings_copy[i]);
    }
}

//=============================================================================

void global_output_1 (struct string_t *strings_arr, char* *strings_copy, int n_Lines, FILE *file_out)
{
//    printf ("\nSorted from left to right:\n");
    fprintf (file_out, "\nSorted from left to right:\n");
//    output_st (strings_copy, n_Lines);                     // output right (sorted) array
    f_output_st (strings_arr, n_Lines, file_out);   // output right (sorted) array in file
}

void global_output_2 (struct string_t *strings_arr, char* *strings_copy, int n_Lines, FILE *file_out)
{
//    printf ("\nSorted from right to left:\n");
    fprintf (file_out, "\nSorted from right to left:\n");
//    output_st (strings_copy, n_Lines);                     // output sorted right to left array in file
    f_output_st (strings_arr, n_Lines, file_out);   // output sorted right to left array in file

//    printf ("\nOriginal:\n");
    fprintf (file_out, "\nOriginal text:\n");
//    output (strings_copy, n_Lines);                   // output original text
    f_output (strings_copy, n_Lines, file_out); // output original text in file
}

//=============================================================================

int if_letter (const char sym)
{
    return ('a' <= sym && sym <= 'z') || ('A' <= sym && sym <= 'Z');
}

//=============================================================================

int number_of_lines (const int sz_File, const char *buf)
{
    int cnt = 0;
    for (int i = 0; i < sz_File - 1; i++)
    {
        if (*(buf + i) == '\n')
        {
            cnt++;
        }
    }

    return cnt;
}

//=============================================================================

void skip_punctuation (char **str_1, char **str_2)
{
    assert (**str_1 && *str_1 && **str_2 && *str_2);

    int ok_1 = 0;
    int ok_2 = 0;

    while ((**str_1 != '\0' && **str_2 != '\0') && (ok_1 == 0 && ok_2 == 0))
    {
        if (!if_letter (**(str_1)))
        {
            (*str_1)--;  assert (*str_1);
        }
        else
            ok_1 = 1;

        if (!if_letter (**(str_2)))
        {
            (*str_2)--;  assert (*str_2);
        }
        else
            ok_2 = 1;
    }
}

//=============================================================================

char* *my_free (char **arr)
{
    free (arr);
    arr = nullptr;

    return arr;
}
