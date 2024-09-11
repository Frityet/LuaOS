add_rules("mode.debug", "mode.release")

add_cflags (
    "-Wall",
    "-Wextra",
    "-Werror",
    "-Wno-unused",
    "-fms-extensions",
    "-Wno-microsoft"
)

add_cflags (
    "-ffreestanding",
    "-fno-stack-protector",
    "-fno-stack-check",
    "-fno-lto",
    "-fno-pie",
    "-fno-pic",
    "-m64",
    "-march=x86-64",
    "-mabi=sysv",
    "-mno-80387",
    "-mno-mmx",
    "-mno-sse",
    "-mno-sse2",
    "-mno-red-zone",
    "-mcmodel=kernel",
    "-target", "x86_64-elf",
    "-nostdinc",
    "-Wno-unused-command-line-argument",
    "-Wanon-enum-enum-conversion",
    "-Wassign-enum",
    "-Wenum-conversion",
    "-Wenum-enum-conversion",
    "-Wno-unused-function",
    "-Wno-unused-parameter",
    "-Wnull-dereference",
    "-Wnull-conversion",
    "-Wnullability-completeness",
    "-Wnullable-to-nonnull-conversion",
    "-Wno-missing-field-initializers",
    "-fno-omit-frame-pointer",
    "-Wno-deprecated-attributes",
    "-fms-extensions",
    "-fblocks"
)

add_asflags (
    "-F", "dwarf",
    -- "-g"
    "-f", "elf64"
)

add_ldflags (
    "-nostdlib",
    "-static",
    "-m", "elf_x86_64",
    "-z", "max-page-size=0x1000",
    "-T", "res/linker.ld",
    "-no-pie"
)

target("terminal")
do
    set_kind("static")
    add_files("extern/terminal/term.c")
    add_files("extern/terminal/backends/framebuffer.c")
end
target_end()

target("ovmf-x64")
do

end
target_end()

--should move to the repo
target("luajit")
do

end
target_end()

target("LuaOS")
do
    set_kind("binary")
    add_deps("terminal")
    set_toolchains("clang") --for now just clang, but this project should be able to be compiled with GCC also
    set_languages("gnulatest")

    add_sysincludedirs("extern")
    add_sysincludedirs("extern/LuaJIT/src")
    add_includedirs("inc")
    set_arch("x86_64")

    add_files("src/**.c")
    add_files("src/**.asm")
end
target_end()
