
## compile:    gcc lis.c -o lis
## run:        ./lis [args]


## Usage: lis [OPTION]... [FILE]...
List information about the FILEs (the current directory by default).
Sort entries alphabetically if none of -cftuvSUX nor --sort is specified.
Implemented w.r.t. ls (GNU coreutils) 8.30

PASS_0 -> [struct beta] -> PASS_1 -> [flagSet] -> PASS_2




## PASS_0 (PARSING):
"lis_cmdParser.c"

The arguments of lis are parsed here.
If invalid arguments, it gives error message,
Otherwise, valid arguments filled in struct beta and passed to PASS_1.

struct beta contents, filled by command parser.

description          |   flag  indOpt  argStr  coeff   num2   num1
---------------------+---------------------------------------------------------------------
normal argument      |   0      -      yes       -      -     -
sort option          |   1      -      yes       -      -     width/tabsize/PATRN
long option no args  |   2     ind      -        -      -     other/help/version
long option PATTERN  |   3     ind     yes       -      -     hide/ignore
long option COLS     |   4     ind      -        -      -     num
long option SIZE     |   5     ind      -      coeff   exp    base
long option WORD     |   6     ind      -        -      -     ind
long option +FORMAT  |   7     ind     yes       -      -     -



## PASS_1 (FLAGING):
"lis_pass1.c"

Reason of pass_1
    1. less/no strcmp.
    2. common flagSet(flaged by shiphen/dhiphen), like. if(char == 'a' || str == "all").
    3. reduce duplication, like. "-l" and "--format=long" both are similar(opt with arg).

According to struct beta, flagSet is filled by pass_1 using charSet.
The files "lis_set_flagSet.txt" and "lis_set_charSet.txt" have more information about flagSet and charSet respectively.



## PASS_2 (no name yet):
"lis_pass2.c"

FILTER_0 = INITIATING
    If help/version exist, print it and stop further processing.

FILTER_1 = RESHAPING
    Discard backup, hidden and pattern matched files (if =PATTERN).
    patternChecker()
        patternMatcher()

FILTER_2 = APPENDING
    fillSet()
        find file inforamtions (if required).

FILTER_3 = SEQUENCING
    sortSet()
        according to sort type
        sort type = name, mtime, ctime, atime, size, version, ext, dir_first
    reverseSet()

FILTER_4 = FORMATING
    format_size()
        sizeAsBlockSizeForm()
        sizeAsHumanReadable()
    format_quotIndcat()
        quoting to names and indicator to directories.
    format_timeStyle()
        full-iso, long-iso, locale, iso.

FILTER_5 = FLOWING
    horizntlPrintFlow()
        print by rows.
        start checking from N lines to 1 line.
    verticalPrintFlow()
        default, print by columns.
        start checking from 1 line to N lines.

FILTER_6 = PRINTING
    Printf contents according to flagSet.
    Random colours with option --color=auto(reduce vision fluctuation).

FILTER_7 = REMOVING
    Remove indicator, quoting from filename(if attached).

FILTER_8 = FREEING
    free memory malloc'd by pass2.


## SIZE_CALCULATION:
{for -s}
    normal:         size as it is.
    block_size:     upperBound(size/blkSize)
    human_readable:     recDiv(roundUp(size/base))

{for -l}
                alpha = upperBound(size/4096)*4
    normal:         alpha as it is.
                beta = alpha*1024
    block_size:     upperBound(beta/blkSize)
    human_readable:     recDiv(roundUp(beta/base))

base        1000/1024 according to --si/-h
recDiv(a/b) recurssive division while a>=b
roundUp     round upto 1 decimal digit.
        after recDiv if ans >= 10, then take upperBound



## REFERENCES-
LIBRARY:(full description at linux manual)
    stdio.h             printf()
    stdlib.h            malloc(), free(), exit()
    string.h            strcpy(), strcat(), strcmp(), strlen()
    dirent.h    struct dirent   opendir(), closedir()
    sys/ioctl.h struct winsize  ioctl()
    sys/stat.h  struct stat stat()
    pwd.h       struct passwd   getpwuid()
    grp.h       struct group    getgrgid()
    time.h      struct tm   gmtime(), strftime()


##########################################
'lis' skips some options of 'ls' as below:
    INCOMPLETE_WITH_FLAGSET_
        --hyperlink
    -Z  --context
    -b  --escape, --quoting-style=escape
    -T  --tabsize
    -W  --width

    INCOMPLETE_WITHOUT_FLAGSET_
    -D  --dired
    -L  --dereference
    -H  --dereference-command-line
        --dereference-command-line-symlink-to-dir


## FOR USER:
    Sometimes the output is correct but useless or unexpected.
    So please report as a bug, Mistakes ready to improve.


## INPUTS AND OUTPUTS:
Only valid options and their arguments are worth for further processing,
    e.g. "--all -a" are valid
         "--za -z" are exited with error message.

lis follows smooth input strategy, i.e. Rule of last action have higher priority,
    e.g. "-aA", "a" is desabled, output is according to rule of "A".
         "--blocksize=2" desable without "-l".
         "-m -l", output is on newline for each entry rather than comma seperated.
         "--" gives list of all long options.
         "--f" print possibilities as '--file-type' '--format' '--full-time'.
         two others are as below.

For options and their arguments, substring is allowed, except ambiguous options,
    e.g. "--fo=lo" considered as "--format=long".

Arguments of option allows synonyms,
    e.g. "--fo=verbose" "--fo=long" both are similer.

Number and unit in argument must be in proper format and unit is within range,
    e.g. "--width=23 --blo=12M -w23" are valid.
         "--blo=1r --wi=f -w -wa" are invalid.
         "--blo=123e" too large, (max valid value is 2^64-1 = 18,446,744,073,709,551,615)

Pattern have only single *, before or after, Also in proper format,
    e.g. "--hide=abc --ignore=*txt -Iabc*" are valid.
         "--hide=ab*cd" misbehave.
         "-I" without argument.

## Output:
    By default, follows vertical print flow.
    Directories are in blue colour, files are in white.
    With "--color=auto", directories with blue background, files with random colours.

Use zoom out(smaller font size) in "lis_set_flagSet.txt" and "lis_set_charSet.txt" files,
if content is exceeding line(May be it misunderstandable).

