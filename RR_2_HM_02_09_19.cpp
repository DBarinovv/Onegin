//=============================================================================
// SORTING TEXT IN LEXICOGRAPHICAL ORDER
//=============================================================================

#include "TXLib.h"
#include <stdio.h>
#include <sys\stat.h>
#include <assert.h>

//=============================================================================

struct string_t
{
    char *str;
    int ln;
};

//=============================================================================

void make_array_of_structs (char **strings_copy, struct strings *strings_arr,
                            char **buf, int n_Lines, int sz_File);

char *read_file (int *sz_File, FILE *file);

//-----------------------------------------------------------------------------

int compare_1 (const char *str_1, const char *str_2);

int compare_2 (const char *str_1, const char *str_2, int len_1, int len_2);

//-----------------------------------------------------------------------------

void quick_sort_forward (struct strings *strings_arr, int left, int right);

void quick_sort_back (struct strings *strings_arr, int left, int right);

void my_swap (char **elem_1, char **elem_2, int *len_1, int *len_2);

void sort_for_two_elem_1 (char **elem_1, char **elem_2, int *len_1, int *len_2);

void sort_for_two_elem_2 (char **elem_1, char **elem_2, int *len_1, int *len_2);

int sort_for_part_1 (struct strings *strings_arr, int left, int right, int mid);

int sort_for_part_2 (struct strings *strings_arr, int left, int right, int mid);

//-----------------------------------------------------------------------------

void output_st (struct strings *strings_arr, int n_Lines);

void output (char* *true_arr, int n_Lines);

void f_output_st (struct strings *strings_arr, int n_Lines, FILE *file_out);

void f_output (char* *strings_copy, int n_Lines, FILE *file_out);

//=============================================================================

int if_letter (const char sym);

int number_of_lines (const int sz_File, const char *buf);

//=============================================================================

//void TESTS_quick_sort_back (char *file_open, char *file_out);
void TESTS_quick_sort_back ();

//=============================================================================

int main ()
{
    printf ("If you want tests, type T, else another:\n");

    char c = '0';
    if ((c = getchar()) == 'T')
    {
//        TESTS_quick_sort_back ("Onegin.txt", "Onegin_out.txt");
        TESTS_quick_sort_back ();
        return 0;
    }

    int sz_File = 0;    // size of file

    FILE *file     = fopen ("Onegin.txt",     "r"); assert (file);
    FILE *file_out = fopen ("Onegin_out.txt", "w"); assert (file_out);

    char *buf = read_file (&sz_File, file);
    int n_Lines = number_of_lines (sz_File, buf) + 1;

    assert (n_Lines >= 0);

    struct strings strings_arr[n_Lines];       // array with pointers to strings

    char* *strings_copy = (char **)calloc (n_Lines, sizeof(char *));     // array with pointers (to display original text)
//    int *ln_Strings = (int *)calloc (n_Lines, sizeof(int));      // array of strings lens

    make_array_of_structs (strings_copy, strings_arr, &buf, n_Lines, sz_File);

    quick_sort_forward (strings_arr, 0, n_Lines - 1);

//    printf ("\nSorted from left to right:\n");
    fprintf (file_out, "\nSorted from left to right:\n");
//    output_st (strings, n_Lines);                     // output right (sorted) array
    f_output_st (strings_arr, n_Lines, file_out);   // output right (sorted) array in file

    quick_sort_back (strings_arr, 0, n_Lines - 1);

//    printf ("\nSorted from right to left:\n");
    fprintf (file_out, "\nSorted from right to left:\n");
//    output_st (strings, n_Lines);                     // output sorted right to left array in file
    f_output_st (strings_arr, n_Lines, file_out);   // output sorted right to left array in file

//    printf ("\nOriginal:\n");
    fprintf (file_out, "\nOriginal text:\n");
//    output (strings_copy, n_Lines);                   // output original text
    f_output (strings_copy, n_Lines, file_out); // output original text in file

    fclose (file_out);
    }
    return 0;
}

//{============================================================================
//! Make array (strings),
//! in which elem [i] is pointer to the start of string number [i] in the text
//}============================================================================

void make_array_of_structs (char **strings_copy, struct strings *strings_arr,
                             char **buf, int n_Lines, int sz_File)
{
//    strings_arr[0].str = *buf;
//    strings_copy[0] = *buf;

    int pos  = 0;
    int line = -1;

    while (pos < sz_File - 1 && line < n_Lines - 1)
    {
        if ((*buf)[pos] == '\0' || (*buf)[pos] == '\n')
        {
            (*buf)[pos] = '\0';
            line++;
            assert(0 <= line && line < n_Lines);
            strings_arr[line].str = (*buf + pos + 1);
            strings_copy[line] = (*buf + pos + 1);
            strings_arr[line - 1].ln = strings_arr[line].str - strings_arr[line - 1].str;
        }
        pos++;
    }
    strings_arr[line].ln = *buf + pos - strings_arr[line].str;
}

//{============================================================================
//! Read file and make buffer for text
//}============================================================================

char *read_file (int *sz_File, FILE *file)
{
    struct stat text = {};

    stat ("Onegin.txt", &text);
    *sz_File = text.st_size;

    char *buf = (char *)calloc (*sz_File + 1, sizeof(char)); // make buffer for text

    fread (buf, sizeof(char), *sz_File, file);

//-----------------------------------------------------------------------------
// find number of lines
//-----------------------------------------------------------------------------
//
//    for (int i = 0; i < *sz_File - 1; i++)
//    {
//        if (*(buf + i) == '\n')
//        {
//            (*n_Lines)++;
//            buf[i] = '\0';
//        }
//    }

//-----------------------------------------------------------------------------

    fclose (file);

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

int compare_2 (const char *str_1, const char *str_2, int len_1, int len_2)
{
//    printf ("1: *(str_1 + len_1) = [%c], *(str_2 + len_2) = [%c]\n", *(str_1 + len_1), *(str_2 + len_2));
    if (len_1 > 0)
        len_1--;
    if (len_2 > 0)
        len_2--;
//    printf ("2: *(str_1 + len_1) = [%c], *(str_2 + len_2) = [%c]\n\n", *(str_1 + len_1), *(str_2 + len_2));

    assert (len_1 >= 0);
    assert (len_2 >= 0);

//-----------------------------------------------------------------------------
// skip punctuation
//-----------------------------------------------------------------------------

    int ok_1 = 0, ok_2 = 0;

    while (len_1 >= 0 && len_2 >= 0 && (ok_1 == 0 || ok_2 == 0))
    {
//            printf ("*(str_1 + len_1) = [%c], *(str_2 + len_2) = [%c]\n", *(str_1 + len_1), *(str_2 + len_2));
        if (if_letter(*(str_1 + len_1)))
        {
//                printf ("*(str_1 + len_1) = [%c], *(str_2 + len_2) = [%c]", *(str_1 + len_1), *(str_2 + len_2));
            ok_1 = 1;
        }
        else
            len_1--;

        if (if_letter(*(str_2 + len_2)))
        {
//            printf ("*(str_1 + len_1) = [%c], *(str_2 + len_2) = [%c]\n", *(str_1 + len_1), *(str_2 + len_2));
            ok_2 = -1;
        }
        else
            len_2--;
    }

//-----------------------------------------------------------------------------
//    printf ("*(str_1 + len_1) = [%c], *(str_2 + len_2) = [%c]\n", *(str_1 + len_1), *(str_2 + len_2));
    for (; len_1 >= 0 && len_2 >= 0 && *(str_1 + len_1) == *(str_2 + len_2); len_1--, len_2--)
    {
        if (*(str_1 + len_1) == '\0')
            return 0;
    }

//    printf ("str_1 = (%s), str_2 = (%s), return = [%d]\n", str_1, str_2, *(str_1 + len_1) - *(str_2 + len_2));
    if (len_1 < 0 && len_2 < 0)
        return 0;
    return *(str_1 + len_1) - *(str_2 + len_2);
}

//============================================================================

void quick_sort_forward (struct strings *strings_arr, int left, int right)
{
    if (right - left + 1 == 2)
    {
        assert (left >= 0 && right >= 0);
        sort_for_two_elem_1 (&(strings_arr[left].str), &(strings_arr[right].str),
                             &(strings_arr[left].ln), &(strings_arr[right].ln));
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

void quick_sort_back (struct strings *strings_arr, int left, int right)
{
    if (right - left + 1 == 2)
    {
        assert (left >= 0 && right >= 0);
        sort_for_two_elem_2 (&strings_arr[left].str, &strings_arr[right].str,
                             &strings_arr[left].ln, &strings_arr[right].ln);
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
    if (compare_1 (*elem_1, *elem_2) > 0)
    {
        my_swap (elem_1, elem_2, len_1, len_2);
    }
}

//{============================================================================
//! Make array right form:
//! All elements < mid located on the left
//! All elements > mid located on the right
//}============================================================================

int sort_for_part_1 (struct strings *strings_arr, int left, int right, int mid)
{
    int res = mid;

    while (left < right)
    {
        assert(left >= 0 && right >= 0 && res >= 0);
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
            if (compare_1 (strings_arr[left].str, strings_arr[res].str) == 0)
                res = right;
            else if (compare_1 (strings_arr[right].str, strings_arr[res].str) == 0)
                res = left;
            my_swap (&strings_arr[left].str, &strings_arr[right].str,
                     &strings_arr[left].ln, &strings_arr[right].ln);
        }
    }
    return res;
}

//{============================================================================
//! Make array right form (but for direction = -1):
//! All elements < mid located on the left
//! All elements > mid located on the right
//}============================================================================

int sort_for_part_2 (struct strings *strings_arr, int left, int right, int mid)
{
    int res = mid;
    assert (res >= 0);

    while (left < right)
    {
        assert(left >= 0 && right >= 0 && res >= 0);
//        printf ("strings_arr[left].str = ( %s ), strings_arr[res].str = ( %s )\n",
//                 strings_arr[left].str, strings_arr[res].str);
//        printf ("Compare_2 = [%d]\n\n", compare_2 (strings_arr[left].str, strings_arr[res].str,
//                          strings_arr[left].ln,  strings_arr[res].ln));
        while (compare_2 (strings_arr[left].str, strings_arr[res].str,
                          strings_arr[left].ln,  strings_arr[res].ln) < 0 && left < right)
        {
            left++;
        }

        while (compare_2 (strings_arr[right].str, strings_arr[res].str,
                          strings_arr[right].ln,  strings_arr[res].ln) > 0 && left < right)
        {
            right--;
        }

        if (left < right)
        {
            if (compare_2 (strings_arr[left].str, strings_arr[res].str,
                           strings_arr[left].ln,  strings_arr[res].ln) == 0)
                res = right;
            else if (compare_2 (strings_arr[right].str, strings_arr[res].str,
                                strings_arr[right].ln, strings_arr[res].ln) == 0)
                res = left;

            if (compare_2 (strings_arr[left].str, strings_arr[right].str,
                                strings_arr[left].ln,  strings_arr[right].ln) == 0)
            {
                left++;
                right--;
            }
            else
            {
                my_swap (&strings_arr[left].str, &strings_arr[right].str,
                         &strings_arr[left].ln, &strings_arr[right].ln);
            }
        }
    }
    return res;
}

//{============================================================================
//! Display array in right order
//}============================================================================

void output_st (struct strings *strings_arr, int n_Lines)
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

void f_output_st (struct strings *strings_arr, int n_Lines, FILE *file_out)
{
    assert (file_out != 0);

    for (int i = 0; i < n_Lines; i++)
    {
        fprintf (file_out, " [%d]: (%s) \n", i, strings_arr[i].str);
    }
}

void f_output (char* *strings_copy, int n_Lines, FILE *file_out)
{
    assert (file_out != 0);

    for (int i = 0; i < n_Lines; i++)
    {
        fprintf (file_out, " [%d]: (%s) \n", i, strings_copy[i]);
    }
}

//=============================================================================

int if_letter (const char sym)
{
    return (('a' <= sym && sym <= 'z') || ('A' <= sym && sym <= 'Z'));
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
//=============================================================================
//=============================================================================

//void TESTS_quick_sort_back (char *file_open, char *file_out);
//    FILE *file = fopen (file_open, "r");
//    FILE *file_out = fopen (file_out, "w");
void TESTS_quick_sort_back ()
{
    struct stat text_1 = {};
    struct stat text_2 = {};
    struct stat text_3 = {};
    struct stat text_4 = {};
    struct stat text_5 = {};

    stat ("T1.txt", &text_1);
    stat ("T2.txt", &text_2);
    stat ("T3.txt", &text_3);
    stat ("T4.txt", &text_4);
    stat ("T5.txt", &text_5);

    int sz_File_1 = text_1.st_size;
    int sz_File_2 = text_2.st_size;
    int sz_File_3 = text_3.st_size;
    int sz_File_4 = text_4.st_size;
    int sz_File_5 = text_5.st_size;


    FILE *file_1 =     fopen ("T1.txt",     "r");   // empty
    FILE *file_out_1 = fopen ("T1_out.txt", "w");

    FILE *file_2 =     fopen ("T2.txt",     "r");   // without letters
    FILE *file_out_2 = fopen ("T2_out.txt", "w");

    FILE *file_3 =     fopen ("T3.txt",     "r");   // sorted right to left
    FILE *file_out_3 = fopen ("T3_out.txt", "w");

    FILE *file_4 =     fopen ("T4.txt",     "r");   // letters + digits in the end
    FILE *file_out_4 = fopen ("T4_out.txt", "w");

    FILE *file_5 =     fopen ("T5.txt",     "r");   // only '\n'
    FILE *file_out_5 = fopen ("T5_out.txt", "w");

    char *buf_1 = (char *)calloc (sz_File_1 + 1, sizeof(char));
    char *buf_2 = (char *)calloc (sz_File_2 + 1, sizeof(char));
    char *buf_3 = (char *)calloc (sz_File_3 + 1, sizeof(char));
    char *buf_4 = (char *)calloc (sz_File_4 + 1, sizeof(char));
    char *buf_5 = (char *)calloc (sz_File_5 + 1, sizeof(char));

    fread (buf_1, sizeof(char), sz_File_1, file_1);
    fread (buf_2, sizeof(char), sz_File_2, file_2);
    fread (buf_3, sizeof(char), sz_File_3, file_3);
    fread (buf_4, sizeof(char), sz_File_4, file_4);
    fread (buf_5, sizeof(char), sz_File_5, file_5);

    int n_Lines_1 = number_of_lines (sz_File_1, buf_1) + 1;
    int n_Lines_2 = number_of_lines (sz_File_2, buf_2) + 1;
    int n_Lines_3 = number_of_lines (sz_File_3, buf_3) + 1;
    int n_Lines_4 = number_of_lines (sz_File_4, buf_4) + 1;
    int n_Lines_5 = number_of_lines (sz_File_5, buf_5) + 1;

    assert (file_out_1);
    assert (file_out_2);
    assert (file_out_3);
    assert (file_out_4);
    assert (file_out_5);

    struct strings strings_arr_1[n_Lines_1];
    struct strings strings_arr_2[n_Lines_2];
    struct strings strings_arr_3[n_Lines_3];
    struct strings strings_arr_4[n_Lines_4];
    struct strings strings_arr_5[n_Lines_5];

    char* *strings_copy_1 = (char **)calloc (n_Lines_1 + 1, sizeof(char *));
    char* *strings_copy_2 = (char **)calloc (n_Lines_2, sizeof(char *));
    char* *strings_copy_3 = (char **)calloc (n_Lines_3, sizeof(char *));
    char* *strings_copy_4 = (char **)calloc (n_Lines_4, sizeof(char *));
    char* *strings_copy_5 = (char **)calloc (n_Lines_5, sizeof(char *));

    make_array_of_structs (strings_copy_1, strings_arr_1, &buf_1, n_Lines_1, sz_File_1);
    make_array_of_structs (strings_copy_2, strings_arr_2, &buf_2, n_Lines_2, sz_File_2);
    make_array_of_structs (strings_copy_3, strings_arr_3, &buf_3, n_Lines_3, sz_File_3);
    make_array_of_structs (strings_copy_4, strings_arr_4, &buf_4, n_Lines_4, sz_File_4);
    make_array_of_structs (strings_copy_5, strings_arr_5, &buf_5, n_Lines_5, sz_File_5);

    quick_sort_back (strings_arr_1, 0, n_Lines_1 - 1);
    quick_sort_back (strings_arr_2, 0, n_Lines_2 - 1);
    quick_sort_back (strings_arr_3, 0, n_Lines_3 - 1);
    quick_sort_back (strings_arr_4, 0, n_Lines_4 - 1);
    quick_sort_back (strings_arr_5, 0, n_Lines_5 - 1);

    printf ("\nFirst - sorted, second - original\n\n");
    printf ("============================================\n");
    output_st (strings_arr_1, n_Lines_1);
    printf ("--------------------------------------------\n");
    output (strings_copy_1, n_Lines_1);
    printf ("============================================\n");

    printf ("============================================\n");
    output_st (strings_arr_2, n_Lines_2);
    printf ("--------------------------------------------\n");
    output (strings_copy_2, n_Lines_2);
    printf ("============================================\n");

    printf ("============================================\n");
    output_st (strings_arr_3, n_Lines_3);
    printf ("--------------------------------------------\n");
    output (strings_copy_3, n_Lines_3);
    printf ("============================================\n");

    printf ("============================================\n");
    output_st (strings_arr_4, n_Lines_4);
    printf ("--------------------------------------------\n");
    output (strings_copy_4, n_Lines_4);
    printf ("============================================\n");

    printf ("============================================\n");
    output_st (strings_arr_5, n_Lines_5);
    printf ("--------------------------------------------\n");
    output (strings_copy_5, n_Lines_5);
    printf ("============================================\n");

//    printf ("============================================\n");
//    f_output_st (strings_arr_1, n_Lines_1, file_out_1);
//    printf ("--------------------------------------------\n");
//    f_output (strings_copy_1, n_Lines_1, file_out_1);
//    printf ("============================================\n");
//
//    printf ("============================================\n");
//    f_output_st (strings_arr_2, n_Lines_2, file_out_2);
//    printf ("--------------------------------------------\n");
//    f_output (strings_copy_2, n_Lines_2, file_out_2);
//    printf ("============================================\n");
//
//    printf ("============================================\n");
//    f_output_st (strings_arr_3, n_Lines_3, file_out_3);
//    printf ("--------------------------------------------\n");
//    f_output (strings_copy_3, n_Lines_3, file_out_3);
//    printf ("============================================\n");
//
//    printf ("============================================\n");
//    f_output_st (strings_arr_4, n_Lines_4, file_out_4);
//    printf ("--------------------------------------------\n");
//    f_output (strings_copy_4, n_Lines_4, file_out_4);
//    printf ("============================================\n");
//
//    printf ("============================================\n");
//    f_output_st (strings_arr_5, n_Lines_5, file_out_5);
//    printf ("--------------------------------------------\n");
//    f_output (strings_copy_5, n_Lines_5, file_out_5);
//    printf ("============================================\n");
//
//    f_output (strings_copy_1, n_Lines_1, file_out_1);
//    f_output (strings_copy_2, n_Lines_2, file_out_2);
//    f_output (strings_copy_3, n_Lines_3, file_out_3);
//    f_output (strings_copy_4, n_Lines_4, file_out_4);
//    f_output (strings_copy_5, n_Lines_5, file_out_5);
//    printf ("============================================");
//    fclose (file_1);
//    fclose (file_2);
//    fclose (file_3);
//    fclose (file_4);
//    fclose (file_5);
//
//    fclose (file_out_1);
//    fclose (file_out_2);
//    fclose (file_out_3);
//    fclose (file_out_4);
//    fclose (file_out_5);
}
