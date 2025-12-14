# CANTram Framework Tools

This directory contains utility tools and templates for working with the CANTram Framework.

## Available Tools

### Library Template

**Location**: `library_template/`

A complete template for creating new libraries for the CANTram Framework. This template includes:

- Standard library structure
- Properly formatted header and implementation files
- Example README with all recommended sections
- Sample usage example
- PlatformIO library manifest (library.json)

**Usage**:

1. Copy the entire `library_template/` directory:
   ```bash
   cp -r tools/library_template lib/YourNewLibrary
   ```

2. Rename files:
   ```bash
   cd lib/YourNewLibrary
   mv src/LibraryName.h src/YourNewLibrary.h
   mv src/LibraryName.cpp src/YourNewLibrary.cpp
   ```

3. Update file contents:
   - Replace all instances of "LibraryName" with your library name
   - Update author information
   - Fill in actual functionality
   - Update README.md with relevant information
   - Customize library.json

4. Implement your library:
   - Add your class members and methods
   - Implement functionality
   - Create meaningful examples
   - Test on target platforms

## Future Tools

### Code Generator (Planned)
Automated code generator for creating libraries from specifications.

### Configuration Tool (Planned)
Web-based tool for configuring CANTram nodes and networks.

### CAN Bus Analyzer (Planned)
Tool for analyzing and debugging CAN bus communications.

### Library Validator (Planned)
Script to validate that libraries meet framework standards.

### Documentation Generator (Planned)
Automated documentation generation for all libraries.

## Contributing Tools

If you create a useful tool for the CANTram Framework, please consider contributing it:

1. Place your tool in this directory
2. Add a README explaining its purpose and usage
3. Update this file to list your tool
4. Submit a pull request

## License

All tools are licensed under GPL-3.0, same as the main framework.
