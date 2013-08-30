#!/bin/bash

ESC_P="$(echo "$1" | sed -e 's/[!()&|]/\\&/g')"
sh -c './dpt --profile=test --search='$ESC_P
