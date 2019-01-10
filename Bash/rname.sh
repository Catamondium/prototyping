#!/bin/bash

function rename() # (path, recurse, dry, verbose)
{ 
	declare -a files=($1/*)
	if [ "${#files[@]}" == 0 ]; then
		exit 0
	fi
	local width
	((width=$(log ${#files[@]}) + 1))
	local i=0
	for f in "${files[@]}"; do
		if [[ -f "$f" ]]; then
			local dest=$(printf "%s/%s-%0*d.%s" "$1" "${1##/*/}" "$width" "$i" "${f#*.}")
			if $4; then echo "$f" "->" "$dest"; fi
			# Suppress 'x to x' error 
			if ! $3; then mv "$f" "$dest" 2> /dev/null; fi
			((i++))
		elif [[ $2 && -d "$f" ]]; then
			rename "$f" $2 $3 $4
		fi
	done
}

function verify() # (path)
{ 
	echo "About to rename inside:" ${1##/*/}
	read -p "Are you sure? " -n 1 -r
	echo
	return $([[ $REPLY =~ ^[Yy]$ ]])
}

function usage()
{
	printf "Batch renamer following DIR-#.ext pattern.\n"
	printf "# is an integer zfilled to width log10 of " 
	printf "the number of files to mv recursing directories\n"
	printf "Usage:\t$0 -[vhfir] [target]\n"
	printf "Options:\n"
	printf "\t-d Dry run, verbosely\n"
	printf "\t-v Verbose\n"
	printf "\t-r Rename recursively\n"
	printf "\t-f Don't ask for approval\n"
	printf "\t-i Require approval\n"
	printf "\t-h Show this usage\n"
	exit 1
}

verbose=false
force=false
recurse=false
dry=false
while getopts "dvhfir" c; do
	case "${c}" in
		d)
			dry=true
			verbose=true
			;;
		v)
			verbose=true
			;;
		r)
			recurse=true
			;;
		f)
			force=true
			;;
		i)
			force=false
			;;
		h | *)
			usage
			;;
	esac
done

path=$(readlink -f $1 2> /dev/null) # suppressed readlink warnings
: ${path:=$(pwd)}                   # default to current working dir

if $dry || $force || verify $path; then
	rename $path $recurse $dry $verbose
fi