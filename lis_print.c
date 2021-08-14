
void printVersion() {
printf("lis version 1.0\n");
printf("Implemented w.r.t. ls (GNU coreutils) 8.30\n");
printf("Copyright (C) under PUCSD Assignments of Software Development\n");
printf("\n");
printf("Lead by Prof. Achyut K. Roy.\n");
printf("Written by Ashok J. Kumawat.\n");
}


void printHelp() {
printf("Usage: lis [OPTION]... [FILE]...\n");
printf("List information about the FILEs (the current directory by default).\n");
printf("Sort entries alphabetically if none of -cftuvSUX nor --sort is specified.\n");
printf("\n");
printf("Mandatory arguments to long options are mandatory for short options too.\n");
printf("  -a, --all                  do not ignore entries starting with .\n");
printf("  -A, --almost-all           do not list implied . and ..\n");
printf("      --author               with -l, print the author of each file\n");
printf("      --block-size=SIZE      with -l, scale sizes by SIZE when printing them;\n");
printf("                               e.g., '--block-size=M'; see SIZE format below\n");
printf("  -B, --ignore-backups       do not list implied entries ending with ~\n");
printf("  -c                         with -lt: sort by, and show, ctime (time of last\n");
printf("                               modification of file status information);\n");
printf("                               with -l: show ctime and sort by name;\n");
printf("                               otherwise: sort by ctime, newest first\n");
printf("  -C                         list entries by columns\n");
printf("      --color[=WHEN]         colorize the output; WHEN can be 'always' (default\n");
printf("                               if omitted), 'auto', or 'never'; more info below\n");
printf("  -d, --directory            list directories themselves, not their contents\n");
printf("  -f                         do not sort, enable -aU, disable -ls --color\n");
printf("  -F, --classify             append indicator (one of */=>@|) to entries\n");
printf("      --file-type            likewise, except do not append '*'\n");
printf("      --format=WORD          across -x, commas -m, horizontal -x, long -l,\n");
printf("                               single-column -1, verbose -l, vertical -C\n");
printf("      --full-time            like -l --time-style=full-iso\n");
printf("  -g                         like -l, but do not list owner\n");
printf("      --group-directories-first\n");
printf("                             group directories before files;\n");
printf("                               can be augmented with a --sort option, but any\n");
printf("                               use of --sort=none (-U) disables grouping\n");
printf("  -G, --no-group             in a long listing, don't print group names\n");
printf("  -h, --human-readable       with -l and -s, print sizes like 1K 234M 2G etc.\n");
printf("      --si                   likewise, but use powers of 1000 not 1024\n");
printf("      --hide=PATTERN         do not list implied entries matching shell PATTERN\n");
printf("                               (overridden by -a or -A)\n");
printf("      --indicator-style=WORD  append indicator with style WORD to entry names:\n");
printf("                               none (default), slash (-p),\n");
printf("                               file-type (--file-type), classify (-F)\n");
printf("  -i, --inode                print the index number of each file\n");
printf("  -I, --ignore=PATTERN       do not list implied entries matching shell PATTERN\n");
printf("  -k, --kibibytes            default to 1024-byte blocks for disk usage;\n");
printf("                               used only with -s and per directory totals\n");
printf("  -l                         use a long listing format\n");
printf("  -m                         fill width with a comma separated list of entries\n");
printf("  -n, --numeric-uid-gid      like -l, but list numeric user and group IDs\n");
printf("  -N, --literal              print entry names without quoting\n");
printf("  -o                         like -l, but do not list group information\n");
printf("  -p, --indicator-style=slash\n");
printf("                             append / indicator to directories\n");
printf("  -q, --hide-control-chars   print ? instead of nongraphic characters\n");
printf("      --show-control-chars   show nongraphic characters as-is (the default,\n");
printf("                               unless program is 'ls' and output is a terminal)\n");
printf("  -Q, --quote-name           enclose entry names in double quotes\n");
printf("      --quoting-style=WORD   use quoting style WORD for entry names:\n");
printf("                               literal, locale, shell, shell-always,\n");
printf("                               shell-escape, shell-escape-always, c, escape\n");
printf("                               (overrides QUOTING_STYLE environment variable)\n");
printf("  -r, --reverse              reverse order while sorting\n");
printf("  -R, --recursive            list subdirectories recursively\n");
printf("  -s, --size                 print the allocated size of each file, in blocks\n");
printf("  -S                         sort by file size, largest first\n");
printf("      --sort=WORD            sort by WORD instead of name: none (-U), size (-S),\n");
printf("                               time (-t), version (-v), extension (-X)\n");
printf("      --time=WORD            with -l, show time as WORD instead of default\n");
printf("                               modification time: atime or access or use (-u);\n");
printf("                               ctime or status (-c); also use specified time\n");
printf("                               as sort key if --sort=time (newest first)\n");
printf("      --time-style=TIME_STYLE  time/date format with -l; see TIME_STYLE below\n");
printf("  -t                         sort by modification time, newest first\n");
printf("  -u                         with -lt: sort by, and show, access time;\n");
printf("                               with -l: show access time and sort by name;\n");
printf("                               otherwise: sort by access time, newest first\n");
printf("  -U                         do not sort; list entries in directory order\n");
printf("  -v                         natural sort of (version) numbers within text\n");
printf("  -x                         list entries by lines instead of by columns\n");
printf("  -X                         sort alphabetically by entry extension\n");
printf("  -1                         list one file per line.  Avoid '\n' with -q or -b\n");
printf("      --help                 display this help and exit\n");
printf("      --version              output version information and exit\n");
printf("\n");
printf("The following options are usually not available to work\n");
printf("      --hyperlink[=WHEN]     hyperlink file names; WHEN can be 'always'\n");
printf("                               (default if omitted), 'auto', or 'never'\n");
printf("  -b, --escape               print C-style escapes for nongraphic characters\n");
printf("  -T, --tabsize=COLS         assume tab stops at each COLS instead of 8\n");
printf("  -w, --width=COLS           set output width to COLS.  0 means no limit\n");
printf("  -Z, --context              print any security context of each file\n");
printf("\n");
printf("The SIZE argument is an integer and optional unit (example: 10K is 10*1024).\n");
printf("Units are K,M,G,T,P,E,Z,Y (powers of 1024) or KB,MB,... (powers of 1000).\n");
printf("\n");
printf("The TIME_STYLE argument can be full-iso, long-iso, iso, locale, or +FORMAT.\n");
printf("FORMAT is interpreted like in date(1).\n");
printf("\n");
printf("Using color to distinguish file view is disabled both by default and\n");
printf("with --color=never.  With --color=auto, lis emits random colors for files.\n");
printf("\n");
printf("Exit status:\n");
printf(" 0  if OK,\n");
printf(" -1 if serious trouble (e.g., cannot access command-line argument).\n");
printf("\n");
}

