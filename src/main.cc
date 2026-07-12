// ALGOL 126 Compiler Core (A126C)
// Copyright (c) 2026 mxreal64
//
// This Source Code Form is subject to the terms of the Mozilla Public 
// License, v. 2.0. If a copy of the MPL was not distributed with this 
// file, You can obtain one at http://mozilla.org.

// src/main.cc
import algol126.lexer;
import algol126.parser;
import algol126.codegen;
import algol126.ast;
import std;

void print_banner() {
    std::println("========================================================");
    std::println("          ALGOL126 COMPILER INTERFACE [A126C]           ");
    std::println("========================================================");
}

int main(int argc, char* argv[]) {
    print_banner();

    if (argc < 2) {
        std::println(std::cerr, "Error: No source file provided.");
        std::println(std::cerr, "Usage: ./a26c <filename.a26 | filename.a68>");
        return 1;
    }
    std::filesystem::path source_path(argv[1]);
    
    if (source_path.extension() != ".a26" && source_path.extension() != ".a68") {
        std::println(std::cerr, "Error: Unsupported extension '{}'. Use .a26 or .a68", source_path.extension().string());
        return 1;
    }

    if (!std::filesystem::exists(source_path)) {
        std::println(std::cerr, "Error: Source file '{}' not found.", source_path.string());
        return 1;
    }

    // Start execution performance benchmarking
    auto start_time = std::chrono::high_resolution_clock::now();

    try {
        std::println("[Driver] Reading source: {}", source_path.string());
        std::ifstream file_stream(source_path);
        std::stringstream buffer;
        buffer << file_stream.rdbuf();
        std::string source_content = buffer.str();

        // Pipeline Stage 1: Lexer Scan
        std::println("[Driver] Running lexical token analysis...");
        Lexer lexer;
        auto tokens = lexer.tokenize(source_content);
        for (const auto& tok : tokens) {
    		std::println("Token: '{}' Type: {}", tok.value, (int)tok.type);
		}
		
        // Pipeline Stage 2: Structural Parse
        std::println("[Driver] Constructing Abstract Syntax Tree...");
        Parser parser(tokens);
        auto ast_root = parser.parse_program();
        
        // Pipeline Stage 3: Native x86_64 Assembly Codegen
        std::println("[Driver] Generating x86_64 assembly optimization target...");
        CodeGenerator codegen;
        std::string raw_assembly = codegen.generate_assembly(ast_root);

        // Pipeline Stage 4: Staging the Workspace
        std::filesystem::create_directory("./build");
        std::filesystem::path target_s_file = "./build/target.s";
        std::filesystem::path output_binary = "./build/" + source_path.stem().string();

        std::ofstream out_file(target_s_file);
        out_file << raw_assembly;
        out_file.close();

        // Pipeline Stage 5: GCC Assembler/Linker Call
        std::println("[Driver] Dispatching host assembler backend optimizer...");
        std::string gcc_command = std::format("gcc -mavx2 {} -o {}", target_s_file.string(), output_binary.string());
        
        int status = std::system(gcc_command.c_str());
        if (status != 0) {
            std::println(std::cerr, "Error: Backend compilation error. GCC assembly execution failed.");
            return 1;
        }

        // Calculate final execution metrics
        auto end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> elapsed = end_time - start_time;

        std::println("\n[Success] Binary compiled successfully!");
        std::println("[Success] Executable ready at: {}", output_binary.string());
        std::println("[Success] Total compile time:  {:.2f} ms\n", elapsed.count());

    } catch (const std::exception& e) {
        std::println(std::cerr, "\nFatal Pipeline Crash: {}", e.what());
        return 1;
    }

    return 0;
}
