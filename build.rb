require "fileutils"

def run(*cmd)
  puts *cmd
  system(*cmd) or abort("Failed: #{cmd}")
end

def help()
  puts "txbe buildscript"
  puts
  puts "--build  or -b:    Compiles the executable."
  puts "--termux or -t:    Compiles the executable fixing termux problems."
  puts "--gdb    or -g:    Compiles a debug bin, if run arg passed, so run with gdb."
  puts "--run    or -r:    Runs the executable."
  puts "--terx11 or -tx11: Runs the executable opening a x11 server at termux-x11 (requires run arg)."
  puts "--help   or -h:    Shows help."
end

def cleanup(pid)
  puts "Shutting down termux-x11..."
  Process.kill("TERM", pid)
  exit
end

option_build = false

option_termux = false
option_tx11= false

option_run = false
option_gdb = false

ARGV.each do |arg|
  case arg
  when "--build", "-b"
    option_build = true
  when "--termux", "-t"
    option_termux = true
  when "--terx11", "-tx11"
    option_tx11 = true
  when "--run", "-r"
    option_run = true
  when "--gdn", "-g"
    option_gdb = true
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
  FileUtils.mkdir_p("build")
  run("nasm -f bin SimpleBootloader.asm -o build/SimpleBootloader.img")
  run(
    "cmake -B build -S . -DBACKEND=stdout " \
    "#{option_gdb ? "-DCMAKE_BUILD_TYPE=Debug" : ""}"
  )
  run("cmake --build build")
end

exec = "build/txbe_e"

if option_termux
  home = ENV["HOME"]
  final_exec_path = "#{home}/temp/c/txbe"
  FileUtils.mkdir_p(final_exec_path)
  FileUtils.cp_r("#{exec}", "#{final_exec_path}/main")
  run("chmod +x #{final_exec_path}/main")
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