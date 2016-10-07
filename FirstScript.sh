export INSTALLDIR="$PWD/tceinstallloc"
echo $TRAVIS_OS_NAME
if [[ "$TRAVIS_OS_NAME" == "linux" ]]; then 
wget --no-check-certificate http://cmake.org/files/v3.4/cmake-3.4.3-Linux-x86_64.tar.gz
tar -xzf cmake-3.4.3-Linux-x86_64.tar.gz
export PATH=$PWD/cmake-3.4.3-Linux-x86_64/bin:$PATH
rm cmake-3.4.3-Linux-x86_64.tar.gz
fi
if [[ "$LLVM_VER_BUILD" == "3.8" -o "$LLVM_VER_BUILD" == "3.9" ]]; then
sed -e "s/cmake -G \"Unix Makefiles\"/cmake -G \"Ninja\"/g" tce/tools/scripts/install_llvm_$LLVM_VER_BUILD.sh > tce/tools/scripts/install_llvm_$LLVM_VER_BUILD.sh.tmp && mv tce/tools/scripts/install_llvm_$LLVM_VER_BUILD.sh.tmp tce/tools/scripts/install_llvm_$LLVM_VER_BUILD.sh
fi

#- if [[ "$TRAVIS_OS_NAME" == "linux" ]]; then wget --no-check-certificate http://llvm.org/releases/3.4.2/clang+llvm-3.4.2-x86_64-unknown-ubuntu12.04.xz ;fi
#- if [[ "$TRAVIS_OS_NAME" == "linux" ]]; then xz -d -c clang+llvm-3.4.2-x86_64-unknown-ubuntu12.04.xz | tar -x ;fi 
#- if [[ "$TRAVIS_OS_NAME" == "linux" ]]; then export PATH=$PWD/clang+llvm-3.4.2-x86_64-unknown-ubuntu12.04/bin:$PATH ;fi
#- if [[ "$TRAVIS_OS_NAME" == "linux" ]]; then rm clang+llvm-3.4.2-x86_64-unknown-ubuntu12.04.xz ;fi

if [[ "$TRAVIS_OS_NAME" == "osx" ]]; then 
brew update;
brew install ccache;
brew install boost;
brew install xerces-c;
brew install llvm;
export PATH=/usr/local/opt/llvm/bin:$PATH
fi

if [ "$CC"  == "clang" ]; then 
ln -s $(which ccache) $HOME/clang && ln -s $(which ccache) $HOME/clang++ && export PATH=$HOME:$PATH;
export CC='clang -Qunused-arguments -fcolor-diagnostics'
export CXX='clang++ -Qunused-arguments -fcolor-diagnostics' 
fi

if [ "$CC"  == "gcc" ]; then 
export CC='gcc-4.8'; export CXX='g++-4.8'
fi
ccache -z -M 500M
./InstallScript.sh
