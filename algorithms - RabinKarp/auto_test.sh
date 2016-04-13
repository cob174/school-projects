#!/bin/bash
#
red='\E[31m'
green='\E[32m'
blue='\E[37m'
#
#
# It creates a hidden director (.testing) in the assignment folder to keep
# the output of each run and the diff results. If you want to manually
# review the results you can look in the folder.
#
test_dir=~/cop4531/proj6/.testing
his_path=~/cop4531/proj6/.his
my_path=~/cop4531/proj6
proj_dir=~/cop4531/proj6

# Export them so we can access them in other shells
export color=$red
export test_dir
export his_path
export my_path
export proj_dir

# Pushd and Popd Quiet
pushdq() { pushd $1 > /dev/null ; }
popdq()  { popd > /dev/null ; }


# If the testing directory doesn't exist, create it first.
if [ ! -d $test_dir ] ; then
  echo "Making .testing directory, running: mkdir -p $test_dir"
  mkdir -p $test_dir
fi

# If the .his directory doesn't exist, create it first.
if [ ! -d $his_path ] ; then
  echo "Making .his directory, running: mkdir -p $his_dir"
  mkdir -p $his_path
  echo "Copying frk.x from area51..."
  cp ~cop4531p/LIB/area51/frk.x $his_path/
  chmod +x $his_path/frk.x
fi

#
# Function to reset screen color to default
colorReset() { tput sgr0 ; }


# Some counts to keep track of failures
success_count=0
failure_count=0

print_main_header()
{
  echo -e "--------------------------------------------------------------------------------"
  echo -e "    $@"
  echo -e "--------------------------------------------------------------------------------"
}

print_header()
{
  echo -e "  ----------------------------------------------------------------------------  "
  echo -e "    $@"
  echo -e "  ----------------------------------------------------------------------------  "
}

create_makefile()
{
  TAB="$(printf '\t')"
  cat > $proj_dir/makefile <<EOF
LIB     = /home/courses/cop4531p/LIB
CPP     = \$(LIB)/cpp
TCPP    = \$(LIB)/tcpp
AREA51  = \$(LIB)/area51
PROJ    = .
INCPATH = -I\$(PROJ) -I\$(CPP) -I\$(TCPP)

CC   = clang++ -std=c++11

VPATH = \$(PROJ):\$(CPP):\$(TCPP)

all: frk.x

frk.x: frk.cpp rk.h
${TAB}\$(CC) \$(INCPATH) -ofrk.x frk.cpp

test: frk.x
${TAB}./auto_test.sh

EOF
}

# If the makefile doesn't exist, create it first.
if [ ! -f $proj_dir/makefile ] ; then
  echo "Creating makefile..."
  create_makefile
fi

# If the make fails, report failure and exit script.
if ! make >> /dev/null 2>&1 ; then
  echo -e "make failed:\n\tRun make manually to find problem and try again.\n\tExiting Script..."
  exit 1
fi


test_frk() {
  $my_path/frk.x $1 "$2" $3 $4 > my.$1.$3.$4.out
  $his_path/frk.x $1 "$2" $3 $4 > his.$1.$3.$4.out
  diff his.$1.$3.$4.out my.$1.$3.$4.out > diff.$1.$3.$4.out 2>&1
  if [ $? -ne 0 ] ; then
    print_header "diff his.$1.$3.$4.out my.$1.$3.$4.out > diff.$1.$3.$4.out"
    cat diff.$1.$3.$4.out
    sleep 1
    let 'failure_count += 1'
  else
    let 'success_count += 1'
  fi
}

# ** arguments:
#    1: string 'pattern'   (required)
#    2: string 'text'      (required)
#    3: int        {0 = silent, 1 = proof, 2 = dump} (optional)
#    4: bool vegas {0 = Monte Carlo, 1 = Las Vegas}  (optional)
# *** try again

search_for="a bb ccc zzzz not_in_the_text a,no-way,b"
search_in="aaa/bbb/cccbbb/ddd/eee/fffeee/ggg/hhh/iiihhh/kkk/lll/mmm/nnn/ooo/zzzz"

pushdq $test_dir

print_main_header "Starting Jared's random tests see auto_test.sh for details"

for s in $search_for ; do
  for disp in 1 2 ; do
    for prob in 0 1 ; do
      test_frk $s "$search_in" $disp $prob 
    done
  done
done 

#
# Found these after submission
print_main_header "Submission tests 3 and 4 (2 of the 4 total submission tests)"

test_frk "XYXYXYXYX"  "XYXYXYXYZXYXYXYXYZXYXYXYXYXYXYXYXYXYXYXYXYXYXYXYXYXY" 1 0
test_frk "ABCABCABCD" "ABCABCABCABCABCABCABCABCABCABCABCABCABCABCABCABCABCABCABCABCABCABCABCDABCABCABCABCABCABCABCABCABCABCABCABCABC" 2 1

# Wrap things up
#
echo
print_main_header "Number of successful tests: $success_count"
print_main_header "    Number of failed tests: $failure_count"

popdq


