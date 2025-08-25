require "fileutils"

def run(*cmd)
  puts *cmd
  system(*cmd) or abort("Failed: #{cmd}")
end

def help()
  puts "txbe buildscript"
  puts
  puts "--build   or -b:    Compiles the executable."
  puts "--android-libso or -also: Compiles the lib.so for android. Requires NDK"
  puts "--termux  or -t:    Compiles the executable fixing termux problems."
  puts "--gdb     or -g:    Compiles a debug bin, if run arg passed, so run with gdb."
  puts "--run     or -r:    Runs the executable."
  puts "--terx11  or -tx11: Runs the executable opening a x11 server at termux-x11 (requires run arg)."
  puts "--install or -i: Runs the executable opening a x11 server at termux-x11 (requires run arg)."
  puts "--help    or -h:    Shows help."
end

def cleanup(pid)
  puts "Shutting down termux-x11..."
  Process.kill("TERM", pid)
  exit
end

option_build = false
option_build_alibso = false

option_termux = false
option_tx11= false

option_run = false
option_gdb = false

option_install = false

ARGV.each do |arg|
  case arg
  when "--build", "-b"
    option_build = true
  when "--android-libso", "-also"
    option_build_alibso = true
  when "--termux", "-t"
    option_termux = true
  when "--terx11", "-tx11"
    option_tx11 = true
  when "--run", "-r"
    option_run = true
  when "--gdb", "-g"
    option_gdb = true
  when "--install", "-i"
    option_install = true
  when "--help", "-h"
    help
    exit 1
  else
    puts "Arg #{arg} is not a valid command."
    puts "use --help to see all commands"
    exit 1
  end
end

if option_build
  if option_build_alibso
    abis = ["armeabi-v7a", "arm64-v8a", "x86", "x86_64"]
    abis.each do |abi|
      build_dir = "build/#{abi}"
      ANDROID_NDK = ENV["ANDROID_NDK"]
      run(
        "cmake -B #{build_dir} -S . " \
        "-DCMAKE_TOOLCHAIN_FILE=#{ANDROID_NDK}/build/cmake/android.toolchain.cmake " \
        "-DCMAKE_INSTALL_PREFIX=#{ENV['PREFIX']} " \
        "-DANDROID_ABI=#{abi} " \
        "-DANDROID_PLATFORM=android-21 " \
        "-DCMAKE_BUILD_TYPE=#{option_gdb ? "Debug" : "Release"}"
      )
      run("cmake --build #{build_dir}")
    end
  else
    FileUtils.mkdir_p("build")
    run("nasm -f bin SimpleBootloader.asm -o build/SimpleBootloader.img")
    run(
      "cmake -B build -S . -DBACKEND=stdout " \
      "-DCMAKE_INSTALL_PREFIX=#{ENV['PREFIX']} " \
      "-DCMAKE_BUILD_TYPE=#{option_gdb ? "Debug" : "Release"}"
    )
    run("cmake --build build")
  end
end

exec = "build/txbe"

if option_termux
  home = ENV["HOME"]
  final_exec_path = "#{home}/temp/c/txbe"
  FileUtils.mkdir_p(final_exec_path)
  FileUtils.cp_r("#{exec}", "#{final_exec_path}/main")
  run("chmod +x #{final_exec_path}/main")
end

if option_install
  run("cmake --install build")
end

exit(0) if not option_run

if not option_termux
  run("chmod +x #{exec}")
  if option_gdb
    run("gdb ./#{exec} --args ./${exec} build/SimpleBootloader.asm")
  else
    run("./#{exec} build/SimpleBootloader.asm")
  end
else
  if not option_tx11
    if option_gdb
      system("gdb #{final_exec_path}/main --args #{final_exec_path}/main build/SimpleBootloader.img")
    else
      run("#{final_exec_path}/main build/SimpleBootloader.img")
    end
  else
    ENV["DISPLAY"] = ":0"

    x11_pid = spawn("termux-x11 :0", :out => $stdout, :err => $stderr)
    Signal.trap("INT") do
      cleanup(x11_pid)
    end

    sleep 5

    if option_gdb
      system("gdb #{final_exec_path}/main --args #{final_exec_path}/main build/SimpleBootloader.img")
    else
      system("#{final_exec_path}/main build/SimpleBootloader.img")
    end
    cleanup(x11_pid)
  end
end