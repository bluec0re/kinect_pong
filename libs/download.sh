#!/bin/bash

set -e

case "$(uname -o)" in
    "GNU/Linux")
        OS="Linux"
    ;;

    "Cygwin")
        OS="Windows"
    ;;
esac

echo "Detected OS: $OS"

case "$(uname -m)" in
    "x86_64" | "amd64")
        ARCH="x64"
    ;;

    "i686" | "i386" | "i486")
        ARCH="x86"
    ;;
esac

echo "Detected Architecture: $ARCH"

URL="https://www.timoschmid.de/nite2/NiTE-$OS-$ARCH-2.2.tar.zip"
FILENAME=$(basename "$URL")

echo "Downloading $FILENAME"
wget -c "$URL" -O "$FILENAME"

echo "Extracting"
unzip -p "$FILENAME" | tar xj

mv "${FILENAME%%.t*}" "Nite2.2"
