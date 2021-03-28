/*====================================================================*
 -  Copyright (C) 2001 Leptonica.  All rights reserved.
 -  This software is distributed in the hope that it will be
 -  useful, but with NO WARRANTY OF ANY KIND.
 -  No author or distributor accepts responsibility to anyone for the
 -  consequences of using this software, or for whether it serves any
 -  particular purpose or works at all, unless he or she says so in
 -  writing.  Everyone is granted permission to copy, modify and
 -  redistribute this source code, for commercial or non-commercial
 -  purposes, with the following restrictions: (1) the origin of this
 -  source code must not be misrepresented; (2) modified versions must
 -  be plainly marked as such; and (3) this notice may not be removed
 -  or altered from any source or modified source distribution.
 *====================================================================*/


/*
 *   sarray.c
 *
 *      Create/Destroy/Copy
 *          SARRAY    *sarrayCreate()
 *          SARRAY    *sarrayCreateWordsFromString()
 *          SARRAY    *sarrayCreateLinesFromString()
 *          void      *sarrayDestroy()
 *          SARRAY    *sarrayCopy()
 *          SARRAY    *sarrayClone()
 *
 *      Add/Remove string
 *          l_int32    sarrayAddString()
 *          l_int32    sarrayExtendArray()
 *          char      *sarrayRemoveString()
 *          l_int32    sarrayClear()
 *
 *      Accessors
 *          l_int32    sarrayGetCount()
 *          char     **sarrayGetArray()
 *          char      *sarrayGetString()
 *          l_int32    sarrayGetRefcount()
 *          l_int32    sarrayChangeRefcount()
 *
 *      Conversion back to string
 *          char      *sarrayToString()
 *          char      *sarrayToStringRange()
 *
 *      Concatenate 2 sarrays
 *          l_int32    sarrayConcatenate()
 *
 *      Convert word sarray to (formatted) line sarray
 *          SARRAY    *sarrayConvertWordsToLines()
 *
 *      Split string on separator list
 *          SARRAY    *sarraySplitString()
 *
 *      Sort
 *          SARRAY    *sarraySort()
 *          l_int32    stringCompareLexical()
 *
 *      Serialize for I/O
 *          SARRAY    *sarrayRead()
 *          SARRAY    *sarrayReadStream()
 *          l_int32    sarrayWrite()
 *          l_int32    sarrayWriteStream()
 *          l_int32    sarrayAppend()
 *
 *      Comments on usage:
 *
 *          These functions are important for efficient manipulation
 *          of string data.  They have been used in leptonica for
 *          generating and parsing text files, and for generating
 *          code for compilation.  The user is responsible for
 *          correctly disposing of strings that have been extracted
 *          from sarrays.
 *
 *            - When you want a string from an Sarray to inspect it, or
 *              plan to make a copy of it later, use sarrayGetString()
 *              with copyflag = 0.  In this case, you must neither free
 *              the string nor put it directly in another array.
 *              We provide the copyflag constant L_NOCOPY, which is 0,
 *              for this purpose:
 *                 str-not-owned = sarrayGetString(sa, index, L_NOCOPY);
 *              To extract a copy of a string, use:
 *                 str-owned = sarrayGetString(sa, index, L_COPY);
 *
 *            - When you want to insert a string that is in one
 *              array into another array (always leaving the first
 *              array intact), you have two options:
 *                 (1) use copyflag = L_COPY to make an immediate copy,
 *                     which you must then add to the second array
 *                     by insertion; namely,
 *                       str-owned = sarrayGetString(sa, index, L_COPY);
 *                       sarrayAddString(sa, str-owned, L_INSERT);
 *                 (2) use copyflag = L_NOCOPY to get another handle to
 *                     the string, in which case you must add
 *                     a copy of it to the second string array:
 *                       str-not-owned = sarrayGetString(sa, index, L_NOCOPY);
 *                       sarrayAddString(sa, str-not-owned, L_COPY).
 *
 *              In all cases, when you use copyflag = L_COPY to extract
 *              a string from an array, you must either free it
 *              or insert it in an array that will be freed later.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "allheaders.h"

static const l_int32  INITIAL_PTR_ARRAYSIZE = 50;     /* n'importe quoi */
static const l_int32  L_BUF_SIZE = 512;


/*--------------------------------------------------------------------------*
 *                   String array create/destroy/copy/extend                *
 *--------------------------------------------------------------------------*/
/*!
 *  sarrayCreate()
 *
 *      Input:  size of string ptr array to be alloc'd
 *              (use 0 for default)
 *      Return: sarray, or null on error
 */
SARRAY *
sarrayCreate(l_int32  n)
{
SARRAY  *sa;

    PROCNAME("sarrayCreate");

    if (n <= 0)
        n = INITIAL_PTR_ARRAYSIZE;

    if ((sa = (SARRAY *)CALLOC(1, sizeof(SARRAY))) == NULL)
        return (SARRAY *)ERROR_PTR("sa not made", procName, NULL);
    if ((sa->array = (char **)CALLOC(n, sizeof(char *))) == NULL)
        return (SARRAY *)ERROR_PTR("ptr array not made", procName, NULL);

    sa->nalloc = n;
    sa->n = 0;
    sa->refcount = 1;
    return sa;
}


/*!
 *  sarrayCreateWordsFromString()
 *
 *      Input:  string
 *      Return: sarray, or null on error
 *
 *  Notes:
 *      (1) This finds the number of word substrings, creates an sarray
 *          of this size, and puts copies of each substring into the sarray.
 */
SARRAY *
sarrayCreateWordsFromString(const char  *string)
{
l_int32  i, nsub, size, inword;
char    *separators;
SARRAY  *sa;

    PROCNAME("sarrayCreateWordsFromString");

    if (!string)
        return (SARRAY *)ERROR_PTR("textstr not defined", procName, NULL);

        /* Find the number of words */
    size = strlen(string);
    nsub = 0;
    separators = " \n\t";
    inword = FALSE;
    for (i = 0; i < size; i++) {
        if (inword == FALSE &&
           (string[i] != ' ' && string[i] != '\t' && string[i] != '\n')) {
           inword = TRUE;
           nsub++;
        }
        else if (inword == TRUE &&
           (string[i] == ' ' || string[i] == '\t' || string[i] == '\n')) {
           inword = FALSE;
        }
    }

    if ((sa = sarrayCreate(nsub)) == NULL)
        return (SARRAY *)ERROR_PTR("sa not made", procName, NULL);
    sarraySplitString(sa, string, separators);

    return sa;
}


/*!
 *  sarrayCreateLinesFromString()
 *
 *      Input:  string
 *              blankflag  (0 to exclude blank lines; 1 to include)
 *      Return: sarray, or null on error
 *
 *  Notes:
 *      (1) This finds the number of line substrings, creates an sarray of
 *          this size, and puts copies of each substring into the sarray.
 */
SARRAY *
sarrayCreateLinesFromString(char    *string,
                            l_int32  blankflag)
{
l_int32  i, nsub, size, startptr;
char    *cstring, *substring;
SARRAY  *sa;

    PROCNAME("sarrayCreateLinesFromString");

    if (!string)
        return (SARRAY *)ERROR_PTR("textstr not defined", procName, NULL);

        /* find the number of lines */
    size = strlen(string);
    nsub = 0;
    for (i = 0; i < size; i++) {
        if (string[i] == '\n')
            nsub++;
    }

    if ((sa = sarrayCreate(nsub)) == NULL)
        return (SARRAY *)ERROR_PTR("sa not made", procName, NULL);

    if (blankflag) {  /* keep blank lines as null strings */
            /* Make a copy for munging */
        if ((cstring = stringNew(string)) == NULL)
            return (SARRAY *)ERROR_PTR("cstring not made", procName, NULL);
            /* We'll insert nulls like strtok */
        startptr = 0;
        for (i = 0; i < size; i++) {
            if (cstring[i] == '\n') {
                cstring[i] = '\0';
                if ((substring = stringNew(cstring + startptr)) == NULL)
                    return (SARRAY *)ERROR_PTR("substring not made",
                                                procName, NULL);
                sarrayAddString(sa, substring, L_INSERT);
/*                fprintf(stderr, "substring = %s\n", substring); */
                startptr = i + 1;
            }
        }
        if (startptr < size) {  /* no newline at end of last line */
            if ((substring = stringNew(cstring + startptr)) == NULL)
                return (SARRAY *)ERROR_PTR("substring not made",
                                            procName, NULL);
            sarrayAddString(sa, substring, L_INSERT);
/*            fprintf(stderr, "substring = %s\n", substring); */
        }
        FREE(cstring);
    }
    else {  /* remove blank lines; use strtok */
        sarraySplitString(sa, string, "\n");
    }

    return sa;
}


/*!
 *  sarrayDestroy()
 *
 *      Input:  &sarray <to be nulled>
 *      Return: void
 *
 *  Notes:
 *      (1) Decrements the ref count and, if 0, destroys the sarray.
 *      (2) Always nulls the input ptr.
 */
void
sarrayDestroy(SARRAY  **psa)
{
l_int32  i;
SARRAY  *sa;

    PROCNAME("sarrayDestroy");

    if (psa == NULL) {
        L_WARNING("ptr address is NULL!", procName);
        return;
    }
    if ((sa = *psa) == NULL)
        return;

    sarrayChangeRefcount(sa, -1);
    if (sarrayGetRefcount(sa) <= 0) {
        if (sa->array) {
            for (i = 0; i < sa->n; i++)
                FREE(sa->array[i]);
            FREE(sa->array);
        }
        FREE(sa);
    }

    *psa = NULL;
    return;
}

        
/*!
 *  sarrayCopy()
 *
 *      Input:  sarray
 *      Return: copy of sarray, or null on error
 */
SARRAY *
sarrayCopy(SARRAY  *sa)
{
l_int32  i;
SARRAY  *csa;

    PROCNAME("sarrayCopy");

    if (!sa)
        return (SARRAY *)ERROR_PTR("sa not defined", procName, NULL);

    if ((csa = sarrayCreate(sa->nalloc)) == NULL)
        return (SARRAY *)ERROR_PTR("csa not made", procName, NULL);

    for (i = 0; i < sa->n; i++)
        sarrayAddString(csa, sa->array[i], L_COPY);

    return csa;
}


/*!
 *  sarrayClone()
 *
 *      Input:  sarray
 *      Return: ptr to same sarray, or null on error
 */
SARRAY *
sarrayClone(SARRAY  *sa)
{
    PROCNAME("sarrayClone");

    if (!sa)
        return (SARRAY *)ERROR_PTR("sa not defined", procName, NULL);
    sarrayChangeRefcount(sa, 1);
    return sa;
}


/*!
 *  sarrayAddString()
 *
 *      Input:  sarray
 *              string  (string to be added)
 *              copyflag (L_INSERT, L_COPY)
 *      Return: 0 if OK, 1 on error
 *
 *  Notes:
 *      (1) Legacy usage decrees that we always use 0 to insert a string
 *          directly and 1 to insert a copy of the string.  The
 *          enums for L_INSERT and L_COPY agree with this convention,
 *          and will not change in the future.
 *      (2) See usage comments at the top of this file.
 */
l_int32
sarrayAddString(SARRAY  *sa,
                char    *string,
                l_int32  copyflag)
{
l_int32  n;

    PROCNAME("sarrayAddString");

    if (!sa)
        return ERROR_INT("sa not defined", procName, 1);
    if (!string)
        return ERROR_INT("string not defined", procName, 1);
    if (copyflag != L_INSERT && copyflag != L_COPY)
        return ERROR_INT("invalid copyflag", procName, 1);
    
    n = sarrayGetCount(sa);
    if (n >= sa->nalloc)
        sarrayExtendArray(sa);

    if (copyflag == L_INSERT)
        sa->array[n] = string;
    else  /* L_COPY */
        sa->array[n] = stringNew(string);
    sa->n++;

    return 0;
}


/*!
 *  sarrayExtendArray()
 *
 *      Input:  sarray
 *      Return: 0 if OK, 1 on error
 */
l_int32
sarrayExtendArray(SARRAY  *sa)
{
    PROCNAME("sarrayExtendArray");

    if (!sa)
        return ERROR_INT("sa not defined", procName, 1);

    if ((sa->array = (char **)reallocNew((void **)&sa->array,
                              sizeof(l_intptr_t) * sa->nalloc,
                              2 * sizeof(l_intptr_t) * sa->nalloc)) == NULL)
            return ERROR_INT("new ptr array not returned", procName, 1);

    sa->nalloc *= 2;
    return 0;
}


/*!
 *  sarrayRemoveString()
 *
 *      Input:  sarray
 *              index (of string within sarray)
 *      Return: removed string, or null on error
 */
char *
sarrayRemoveString(SARRAY  *sa,
                   l_int32  index)
{
char    *string;
char   **array;
l_int32  i, n, nalloc;

    PROCNAME("sarrayRemoveString");

    if (!sa)
        return (char *)ERROR_PTR("sa not defined", procName, NULL);
    
    if ((array = sarrayGetArray(sa, &nalloc, &n)) == NULL)
        return (char *)ERROR_PTR("array not returned", procName, NULL);

    if (index < 0 || index >= n)
        return (char *)ERROR_PTR("array index out of bounds", procName, NULL);

    string = array[index];

        /* If removed string is not at end of array, shift
         * to fill in, maintaining original ordering.
         * Note: if we didn't care about the order, we could
         * put the last string array[n - 1] directly into the hole.  */
    for (i = index; i < n - 1; i++)
        array[i] = array[i + 1];

    sa->n--;
    return string;
}


/*!
 *  sarrayClear()
 *
 *      Input:  sarray
 *      Return: 0 if OK; 1 on error
 */
l_int32
sarrayClear(SARRAY  *sa)
{
l_int32  i;

    PROCNAME("sarrayClear");

    if (!sa)
        return ERROR_INT("sa not defined", procName, 0);
    for (i = 0; i < sa->n; i++) {  /* free strings and null ptrs */
        FREE(sa->array[i]);
        sa->array[i] = NULL;
    }
    sa->n = 0;
}

        
/*----------------------------------------------------------------------*
 *                               Accessors                              *
 *----------------------------------------------------------------------*/
/*!
 *  sarrayGetCount()
 *
 *      Input:  sarray
 *      Return: count, or 0 if no strings or on error
 */
l_int32
sarrayGetCount(SARRAY  *sa)
{
    PROCNAME("sarrayGetCount");

    if (!sa)
        return ERROR_INT("sa not defined", procName, 0);
    return sa->n;
}
        

/*!
 *  sarrayGetArray()
 *
 *      Input:  sarray
 *              &nalloc  (<optional return> number allocated string ptrs)
 *              &n  (<optional return> number allocated strings)
 *      Return: ptr to string array, or null on error
 *
 *  Notes:
 *      (1) Caution: the returned array is not a copy, so caller
 *          must not destroy it!
 */
char **
sarrayGetArray(SARRAY   *sa,
               l_int32  *pnalloc,
               l_int32  *pn)
{
char  **array;

    PROCNAME("sarrayGetArray");

    if (!sa)
        return (char **)ERROR_PTR("sa not defined", procName, NULL);

    array = sa->array;
    if (pnalloc) *pnalloc = sa->nalloc;
    if (pn) *pn = sa->n;

    return array;
}


/*!
 *  sarrayGetString()
 *
 *      Input:  sarray
 *              index   (to the index-th string)
 *              copyflag  (L_NOCOPY or L_COPY)
 *      Return: string, or null on error
 *
 *  Notes:
 *      (1) Legacy usage decrees that we always use 0 to get the
 *          pointer to the string itself, and 1 to get a copy of
 *          the string.
 *      (2) See usage comments at the top of this file.
 *      (3) To get a pointer to the string itself, use for copyflag:
 *             L_NOCOPY or 0 or FALSE
 *          To get a copy of the string, use for copyflag:
 *             L_COPY or 1 or TRUE
 *          The const values of L_NOCOPY and L_COPY are guaranteed not
 *          to change.
 */
char *
sarrayGetString(SARRAY  *sa,
                l_int32  index,
                l_int32  copyflag)
{
    PROCNAME("sarrayGetString");

    if (!sa)
        return (char *)ERROR_PTR("sa not defined", procName, NULL);
    if (index < 0 || index >= sa->n)
        return (char *)ERROR_PTR("index not valid", procName, NULL);
    if (copyflag != L_NOCOPY && copyflag != L_COPY)
        return (char *)ERROR_PTR("invalid copyflag", procName, NULL);

    if (copyflag == L_NOCOPY)
        return sa->array[index];
    else  /* L_COPY */
        return stringNew(sa->array[index]);
}


/*!
 *  sarrayGetRefCount()
 *
 *      Input:  sarray
 *      Return: refcount, or UNDEF on error
 */
l_int32
sarrayGetRefcount(SARRAY  *sa)
{
    PROCNAME("sarrayGetRefcount");

    if (!sa)
        return ERROR_INT("sa not defined", procName, UNDEF);
    return sa->refcount;
}


/*!
 *  sarrayChangeRefCount()
 *
 *      Input:  sarray
 *              delta (change to be applied)
 *      Return: 0 if OK, 1 on error
 */
l_int32
sarrayChangeRefcount(SARRAY  *sa,
		     l_int32  delta)
{
    PROCNAME("sarrayChangeRefcount");

    if (!sa)
        return ERROR_INT("sa not defined", procName, UNDEF);
    sa->refcount += delta;
    return 0;
}


/*----------------------------------------------------------------------*
 *                      Conversion to string                           *
 *----------------------------------------------------------------------*/
/*!
 *  sarrayToString()
 *
 *      Input:  sarray
 *              addnlflag (flag: 0 adds nothing to each substring
 *                               1 adds '\n' to each substring
 *                               2 adds ' ' to each substring)
 *      Return: dest string, or null on error
 *
 *  Notes:
 *      (1) Concatenates all the strings in the sarray, preserving
 *          all white space.
 *      (2) If addnlflag != 0, adds either a '\n' or a ' ' after
 *          each substring.
 *      (3) This function was NOT implemented as:
 *            for (i = 0; i < n; i++)
 *                     strcat(dest, sarrayGetString(sa, i, L_NOCOPY));
 *          Do you see why?
 */
char *
sarrayToString(SARRAY  *sa,
               l_int32  addnlflag)
{
    PROCNAME("sarrayToString");

    if (!sa)
        return (char *)ERROR_PTR("sa not defined", procName, NULL);

    return sarrayToStringRange(sa, 0, 0, addnlflag);
}


/*!
 *  sarrayToStringRange()
 *
 *      Input: sarray
 *             first  (index of first string to use; starts with 0)
 *             nstrings (number of strings to append into the result; use
 *                       0 to append to the end of the sarray)
 *             addnlflag (flag: 0 adds nothing to each substring
 *                              1 adds '\n' to each substring
 *                              2 adds ' ' to each substring)
 *      Return: dest string, or null on error
 *
 *  Notes:
 *      (1) Concatenates the specified strings inthe sarray, preserving
 *          all white space.
 *      (2) If addnlflag != 0, adds either a '\n' or a ' ' after
 *          each substring.
 */
char *
sarrayToStringRange(SARRAY  *sa,
                    l_int32  first,
                    l_int32  nstrings,
                    l_int32  addnlflag)
{
char    *dest, *src;
l_int32  n, i, last, size, index, len;

    PROCNAME("sarrayToStringRange");

    if (!sa)
        return (char *)ERROR_PTR("sa not defined", procName, NULL);

    n = sarrayGetCount(sa);
    if (first < 0 || first >= n)
        return (char *)ERROR_PTR("first not valid", procName, NULL);
    if (nstrings == 0 || (nstrings > n - first))
        nstrings = n - first;  /* no overflow */
    last = first + nstrings - 1;

    size = 0;
    for (i = first; i <= last; i++) 
        size += strlen(sarrayGetString(sa, i, L_NOCOPY)) + 2;

    if ((dest = (char *)CALLOC(size + 1, sizeof(char))) == NULL)
        return (char *)ERROR_PTR("dest not made", procName, NULL);

    index = 0;
    for (i = first; i <= last; i++) {
        src = sa->array[i];
        len = strlen(src);
        memcpy(dest + index, src, len);
        index += len;
        if (addnlflag == 1) {
            dest[index] = '\n';
            index++;
        }
        else if (addnlflag == 2) {
            dest[index] = ' ';
            index++;
        }
    }

    return dest;
}


/*----------------------------------------------------------------------*
 *                      Concatenate 2 sarrays                           *
 *----------------------------------------------------------------------*/
/*!
 *  sarrayConcatenate()
 *
 *      Input:  sarray1  (to be added to)
 *              sarray2  (append to sarray1)
 *      Return: 0 if OK, 1 on error
 *
 *  Notes:
 *      (1) Copies of the strings in sarray2 are added to sarray1.
 */
l_int32
sarrayConcatenate(SARRAY  *sa1,
                  SARRAY  *sa2)
{
char    *string;
l_int32  n, i;

    PROCNAME("sarrayConcatenate");

    if (!sa1)
        return ERROR_INT("sa1 not defined", procName, 1);
    if (!sa2)
        return ERROR_INT("sa2 not defined", procName, 1);

    n = sarrayGetCount(sa2);
    for (i = 0; i < n; i++) {
        string = sarrayGetString(sa2, i, L_NOCOPY);
        sarrayAddString(sa1, string, L_COPY);
    }

    return 0;
}


/*----------------------------------------------------------------------*
 *                   Convert word sarray to line sarray                 *
 *----------------------------------------------------------------------*/
/*! 
 *  sarrayConvertWordsToLines()
 *
 *      Input:  sa  (sa of individual words)
 *              linesize  (max num of chars in each line)
 *      Return: saout (sa of formatted lines), or null on error
 *
 *  This is useful for re-typesetting text to a specific maximum
 *  line length.  The individual words in the input sarray
 *  are concatenated into textlines.  An input word string of zero
 *  length is taken to be a paragraph separator.  Each time
 *  such a string is found, the current line is ended and 
 *  a new line is also produced that contains just the
 *  string of zero length ("").  When the output sarray
 *  of lines is eventually converted to a string with newlines
 *  (typically) appended to each line string, the empty
 *  strings are just converted to newlines, producing the visible
 *  paragraph separation.
 *
 *  What happens when a word is larger than linesize?
 *  We write it out as a single line anyway!  Words preceding
 *  or following this long word are placed on lines preceding
 *  or following the line with the long word.  Why this choice?
 *  Long "words" found in text documents are typically URLs, and
 *  it's often desirable not to put newlines in the middle of a URL.
 *  The text display program (e.g., text editor) will typically
 *  wrap the long "word" to fit in the window.
 */
SARRAY *
sarrayConvertWordsToLines(SARRAY  *sa,
                          l_int32  linesize)
{
char    *wd, *strl;
l_int32  n, i, len, totlen;
SARRAY  *sal, *saout;

    PROCNAME("sarrayConvertWordsToLines");

    if (!sa)
        return (SARRAY *)ERROR_PTR("sa not defined", procName, NULL);

    if ((saout = sarrayCreate(0)) == NULL)
        return (SARRAY *)ERROR_PTR("saout not defined", procName, NULL);

    n = sarrayGetCount(sa);
    totlen = 0;
    sal = NULL;
    for (i = 0; i < n; i++) {
        if (!sal) {
            if ((sal = sarrayCreate(0)) == NULL)
                return (SARRAY *)ERROR_PTR("sal not made", procName, NULL);
        }
        wd = sarrayGetString(sa, i, L_NOCOPY);
        len = strlen(wd);
        if (len == 0) {  /* end of paragraph: end line & insert blank line */
            if (totlen > 0) {
                strl = sarrayToString(sal, 2);
                sarrayAddString(saout, strl, L_INSERT);
            }
            sarrayAddString(saout, "", L_COPY);
            sarrayDestroy(&sal);
            totlen = 0;
        }
        else if (totlen == 0 && len + 1 > linesize) {  /* long word! */
            sarrayAddString(saout, wd, L_COPY);  /* copy to one line */
        }
        else if (totlen + len + 1 > linesize) {  /* end line & start new one */
            strl = sarrayToString(sal, 2);
            sarrayAddString(saout, strl, L_INSERT);
            sarrayDestroy(&sal);
            if ((sal = sarrayCreate(0)) == NULL)
                return (SARRAY *)ERROR_PTR("sal not made", procName, NULL);
            sarrayAddString(sal, wd, L_COPY);
            totlen = len + 1;
        }
        else {   /* add to current line */
            sarrayAddString(sal, wd, L_COPY);
            totlen += len + 1;
        }
    }
    if (totlen > 0) {   /* didn't end with blank line; output last line */
        strl = sarrayToString(sal, 2);
        sarrayAddString(saout, strl, L_INSERT);
        sarrayDestroy(&sal);
    }

    return saout;

}


/*----------------------------------------------------------------------*
 *                    Split string on separator list                    *
 *----------------------------------------------------------------------*/
/*
 *  sarraySplitString()
 *
 *      Input:  sa (to append to; typically empty initially)
 *              str (string to split; not changed)
 *              separators (characters that split input string)
 *      Return: 0 if OK, 1 on error.
 *
 *  Notes:
 *      (1) This uses strtokSafe().  See the notes there in utils.c.
 */
l_int32
sarraySplitString(SARRAY      *sa,
                  const char  *str,
                  const char  *separators)
{
char  *cstr, *substr, *saveptr;

    PROCNAME("sarraySplitString");

    if (!sa)
        return ERROR_INT("sa not defined", procName, 1);
    if (!str)
        return ERROR_INT("str not defined", procName, 1);
    if (!separators)
        return ERROR_INT("separators not defined", procName, 1);

    cstr = stringNew(str);  /* preserves const-ness of input str */
    substr = strtokSafe(cstr, separators, &saveptr);
    if (substr)
        sarrayAddString(sa, substr, L_INSERT);
    while ((substr = strtokSafe(NULL, separators, &saveptr)))
        sarrayAddString(sa, substr, L_INSERT);
    FREE(cstr);

    return 0;
}


/*----------------------------------------------------------------------*
 *                                   Sort                               *
 *----------------------------------------------------------------------*/
/*!
 *  sarraySort()
 *
 *      Input:  saout (output sarray; can be NULL or equal to sain)
 *              sain (input sarray)
 *              sortorder (L_SORT_INCREASING or L_SORT_DECREASING)
 *      Return: saout (output sarray, sorted by ascii value), or null on error
 *
 *  Notes:
 *      (1) Set saout = sain for in-place; otherwise, set naout = NULL.
 *      (2) Shell sort, modified from K&R, 2nd edition, p.62.
 *          Slow but simple O(n logn) sort.
 */
SARRAY *
sarraySort(SARRAY  *saout,
           SARRAY  *sain,
           l_int32  sortorder)
{
char   **array;
char    *tmp;
l_int32  n, i, j, gap;

    PROCNAME("sarraySort");

    if (!sain)
        return (SARRAY *)ERROR_PTR("sain not defined", procName, NULL);

        /* Make saout if necessary; otherwise do in-place */
    if (!saout)
        saout = sarrayCopy(sain);
    else if (sain != saout)
        return (SARRAY *)ERROR_PTR("invalid: not in-place", procName, NULL);
    array = saout->array;  /* operate directly on the array */
    n = sarrayGetCount(saout);

        /* Shell sort */
    for (gap = n/2; gap > 0; gap = gap / 2) {
        for (i = gap; i < n; i++) {
            for (j = i - gap; j >= 0; j -= gap) {
                if ((sortorder == L_SORT_INCREASING &&
                     stringCompareLexical(array[j], array[j + gap])) ||
                    (sortorder == L_SORT_DECREASING &&
                     stringCompareLexical(array[j + gap], array[j])))
                {
                    tmp = array[j];
                    array[j] = array[j + gap];
                    array[j + gap] = tmp;
                }
            }
        }
    }

    return saout;
}


/*!
 *  stringCompareLexical()
 *
 *      Input:  str1
 *              str2
 *      Return: 1 if str1 > str2 (lexically); 0 otherwise
 *
 *  Notes:
 *      (1) If the lexical values are identical, return a 0, to
 *          indicate that no swapping is required to sort the strings.
 */
l_int32
stringCompareLexical(const char *str1,
                     const char *str2)
{
l_int32  i, len1, len2, len;

    PROCNAME("sarrayCompareLexical");

    if (!str1)
        return ERROR_INT("str1 not defined", procName, 1);
    if (!str2)
        return ERROR_INT("str2 not defined", procName, 1);

    len1 = strlen(str1);
    len2 = strlen(str2);
    len = L_MIN(len1, len2);

    for (i = 0; i < len; i++) {
        if (str1[i] == str2[i])
            continue;
        if (str1[i] > str2[i])
            return 1;
        else
            return 0;
    }

    if (len1 > len2)
        return 1;
    else
        return 0;
}


/*----------------------------------------------------------------------*
 *                           Serialize for I/O                          *
 *----------------------------------------------------------------------*/
/*!
 *  sarrayRead()
 *
 *      Input:  filename
 *      Return: sarray, or null on error
 */
SARRAY *
sarrayRead(const char  *filename)
{
FILE    *fp;
SARRAY  *sa;

    PROCNAME("sarrayRead");

    if (!filename)
        return (SARRAY *)ERROR_PTR("filename not defined", procName, NULL);

    if ((fp = fopenReadStream(filename)) == NULL)
        return (SARRAY *)ERROR_PTR("stream not opened", procName, NULL);

    if ((sa = sarrayReadStream(fp)) == NULL) {
        fclose(fp);
        return (SARRAY *)ERROR_PTR("sa not read", procName, NULL);
    }

    fclose(fp);
    return sa;
}


/*!
 *  sarrayReadStream()
 *
 *      Input:  stream
 *      Return: sarray, or null on error
 *
 *  Notes:
 *      (1) We store the size of each string along with the string.
 *      (2) This allows a string to have embedded newlines.  By reading
 *          the entire string, as determined by its size, we are
 *          not affected by any number of embedded newlines.
 */
SARRAY *
sarrayReadStream(FILE  *fp)
{
char    *bigbuf;
char    *stringbuf;
l_int32  i, n, size, index, bufsize, ret, version;
SARRAY  *sa;

    PROCNAME("sarrayReadStream");

    if (!fp)
        return (SARRAY *)ERROR_PTR("stream not defined", procName, NULL);

    ret = fscanf(fp, "\nSarray Version %d\n", &version);
    if (ret != 1)
        return (SARRAY *)ERROR_PTR("not an sarray file", procName, NULL);
    if (version != SARRAY_VERSION_NUMBER)
        return (SARRAY *)ERROR_PTR("invalid sarray version", procName, NULL);
    fscanf(fp, "Number of strings = %d\n", &n);

    if ((sa = sarrayCreate(n)) == NULL)
        return (SARRAY *)ERROR_PTR("sa not made", procName, NULL);
    bufsize = L_BUF_SIZE + 1;
    if ((stringbuf = (char *)CALLOC(bufsize, sizeof(char))) == NULL)
        return (SARRAY *)ERROR_PTR("stringbuf not made", procName, NULL);

    for (i = 0; i < n; i++) {
	    /* Get the size of the stored string */
        fscanf(fp, "%d[%d]:", &index, &size);
	    /* Expand the string buffer if necessary */
	if (size > bufsize - 5) {
            FREE(stringbuf);
	    bufsize = (l_int32)(1.5 * size);
            stringbuf = (char *)CALLOC(bufsize, sizeof(char));
	}
	    /* Read the stored string, plus leading spaces and trailing \n */
	fread(stringbuf, 1, size + 3, fp);
	    /* Remove the \n that was added by sarrayWriteStream() */
	stringbuf[size + 2] = '\0';
	    /* Copy it in, skipping the 2 leading spaces */
        sarrayAddString(sa, stringbuf + 2, L_COPY);
    }
    fscanf(fp, "\n");

    FREE(stringbuf);
    return sa;
}


/*!
 *  sarrayWrite()
 *
 *      Input:  filename
 *              sarray
 *      Return: 0 if OK; 1 on error
 */
l_int32
sarrayWrite(const char  *filename,
            SARRAY      *sa)
{
FILE  *fp;

    PROCNAME("sarrayWrite");

    if (!filename)
        return ERROR_INT("filename not defined", procName, 1);
    if (!sa)
        return ERROR_INT("sa not defined", procName, 1);

    if ((fp = fopen(filename, "w")) == NULL)
        return ERROR_INT("stream not opened", procName, 1);

    if (sarrayWriteStream(fp, sa))
        return ERROR_INT("sa not written to stream", procName, 1);
    
    fclose(fp);
    return 0;
}


/*!
 *  sarrayWriteStream()
 *
 *      Input:  stream
 *              sarray
 *      Returns 0 if OK; 1 on error
 *
 *  Notes:
 *      (1) This appends a '\n' to each string, which is stripped
 *          off by sarrayReadStream().
 */
l_int32
sarrayWriteStream(FILE    *fp,
                  SARRAY  *sa)
{
l_int32  i, n, len;

    PROCNAME("sarrayWriteStream");

    if (!fp)
        return ERROR_INT("stream not defined", procName, 1);
    if (!sa)
        return ERROR_INT("sa not defined", procName, 1);

    n = sarrayGetCount(sa);
    fprintf(fp, "\nSarray Version %d\n", SARRAY_VERSION_NUMBER);
    fprintf(fp, "Number of strings = %d\n", n);
    for (i = 0; i < n; i++) {
        len = strlen(sa->array[i]);
        fprintf(fp, "  %d[%d]:  %s\n", i, len, sa->array[i]);
    }
    fprintf(fp, "\n");

    return 0;
}


/*!
 *  sarrayAppend()
 *
 *      Input:  filename
 *              sarray
 *      Return: 0 if OK; 1 on error
 */
l_int32
sarrayAppend(const char  *filename,
             SARRAY      *sa)
{
FILE  *fp;

    PROCNAME("sarrayAppend");

    if (!filename)
        return ERROR_INT("filename not defined", procName, 1);
    if (!sa)
        return ERROR_INT("sa not defined", procName, 1);

    if ((fp = fopen(filename, "a")) == NULL)
        return ERROR_INT("stream not opened", procName, 1);

    if (sarrayWriteStream(fp, sa))
        return ERROR_INT("sa not appended to stream", procName, 1);
    
    fclose(fp);
    return 0;
}

