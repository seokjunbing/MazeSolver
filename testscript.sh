#!/bin/bash


{ for (( i=0; i<=4; i++ )); do
        for (( j=2; j<=2; j++ )); do
                for (( k=0; k<=1; k++ )); do
                        ./startup -n$j -d$i -hpierce.cs.dartmouth.edu -a$k || exit -1
                        echo ""
                done
        done
done
} | grep "Size\|Algo\|user\|Moves"
