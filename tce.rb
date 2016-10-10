
class Tce < Formula
  desc 'TTA-based Co-Design Environment'
  homepage 'http://tce.cs.tut.fi/'
  url 'https://github.com/jfmherokiller/tce/archive/master.zip'
  version '1.18'
  sha256 ''
  depends_on 'boost'
  depends_on 'automake' => :build
  depends_on 'autoconf' => :build
  depends_on 'wxwidgets'
  depends_on 'xerces-c'
  depends_on 'sqlite'
  depends_on :x11 # if your formula requires any X11/XQuartz components
  depends_on :python
  depends_on 'libedit'
  depends_on 'pkg-config' => :run
  #depends_on 'wxgtk'
  depends_on 'libtool'
  depends_on 'cmake'

  def install

    # ENV.deparallelize  # if your formula fails when building in parallel

    # Remove unrecognized options if warned by configure
    system './tce/tools/scripts/install_llvm_3.8.1.sh', "#{prefix}/llvm"
    
    ENV.prepend_create_path 'PATH', "#{prefix}/llvm/bin"
    ENV.set
    cd 'tce' do
      system './configure', '--disable-debug',
             '--disable-dependency-tracking',
             '--disable-silent-rules',
             "--prefix=#{prefix}"
      system 'make', 'install'
    end
  end

  test do
    # `test do` will create, run in and delete a temporary directory.
    #
    # This test will fail and we won't accept that! It's enough to just replace
    # "false" with the main program this formula installs, but it'd be nice if you
    # were more thorough. Run the test with `brew test tce`. Options passed
    # to `brew install` such as `--HEAD` also need to be provided to `brew test`.
    #
    # The installed folder is not in the path, so use the entire path to any
    # executables being tested: `system "#{bin}/program", "do", "something"`.
    system 'false'
  end
end
