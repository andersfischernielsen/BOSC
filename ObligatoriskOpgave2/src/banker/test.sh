./banker < input.txt
if [ $? -eq 0 ] ;
    then echo "Banker with input.txt succeeded!"
else
    echo "Banker ended with error code: $?"
fi

./banker < input2.txt
if [ $? -eq 0 ] ;
    then echo "Banker with input2.txt succeeded!"
else
    echo "Banker ended with error code: $?"
fi

./banker < wronginput.txt
if [ $? -eq 0 ] ;
    then echo "Banker with wronginput.txt succeeded!"
else
    echo "Banker ended with error code: $?"
fi
