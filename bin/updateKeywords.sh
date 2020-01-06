#! /bin/bash

CMD_NAME="$0"
function usage()
{
    echo "Usage: $CMD_NAME [OPTIONS]" >&2
    echo "Generate the ewoms-eclio C++ source code files for keywords based" >&2
    echo "on the respective JSON keyword definitions." >&2
    echo >&2
    echo "Recognized options are: " >&2
    echo "   --genkw=CMD        Path to the 'genkw' code generator from ewoms-eclio " >&2
}

GENKW=""

for CUR_ARG in $@; do
    case "$CUR_ARG" in
        "--genkw="*)
            GENKW=${CUR_ARG:8}
            ;;

        *)
            echo "Unrecognized option '$CUR_ARG'" >&2
            echo >&2
            usage
            exit 1
            ;;

    esac
done

if test -z "$GENKW"; then
    # try to guess the location of the "genkw" command
    for CAND in "./build-cmake/genkw" "./bin/genkw" "/usr/bin/genkw" "/usr/local/bin/genkw"; do
        if test -x "./build-cmake/genkw"; then
            GENKW="$CAND"
            break
        fi
    done
fi

if ! test -x "$GENKW"; then
    echo "The \"genkw\" code generator was not found" >&2
    echo >&2
    usage
    exit 1
fi


echo "Using keyword generator '$GENKW'"

echo " Generating keyword list file"

I=0
for CUR_FILE in $(find "share/eclkeywords/" -type f | grep "/[A-Z_0-9]*$"); do
    if test "$I" = "0"; then
        echo -n "$CUR_FILE" > keyword_list.argv
    else
        echo -n ";$CUR_FILE" >> keyword_list.argv
    fi
    I=$((I + 1))
done
echo >> keyword_list.argv

echo "  $I keyword definition files found"

echo " Running code generator"

$GENKW \
    "keyword_list.argv" \
    "./ewoms/eclio/parser/parserkeywords" \
    "./ewoms/eclio/parser/parserkeywordsinit.cc" \
    "./" \
    "ewoms/eclio/parser/parserkeywords"
