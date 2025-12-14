# Contributing to CANTram Framework

Thank you for your interest in contributing to the CANTram Framework! This document provides guidelines for contributing to the project.

## Code of Conduct

By participating in this project, you agree to maintain a respectful and collaborative environment for all contributors.

## How to Contribute

### Reporting Bugs

If you find a bug, please create an issue with:
- Clear description of the problem
- Steps to reproduce
- Expected vs actual behavior
- Platform/hardware information
- Code example if applicable

### Suggesting Features

Feature suggestions are welcome! Please create an issue describing:
- The feature you'd like to see
- Use case and benefits
- Possible implementation approach

### Contributing Code

1. **Fork the repository**
   ```bash
   git clone https://github.com/LeoWatti99/CANTram-Framework.git
   ```

2. **Create a feature branch**
   ```bash
   git checkout -b feature/your-feature-name
   ```

3. **Make your changes**
   - Follow the coding standards (see docs/development.md)
   - Write tests for new functionality
   - Update documentation
   - Add examples if appropriate

4. **Test your changes**
   - Test on relevant platforms
   - Ensure existing tests pass
   - Add new tests for new features

5. **Commit your changes**
   ```bash
   git add .
   git commit -m "Add feature: description"
   ```

6. **Push to your fork**
   ```bash
   git push origin feature/your-feature-name
   ```

7. **Create a Pull Request**
   - Describe your changes
   - Reference any related issues
   - Include test results if applicable

## Coding Standards

### Style Guidelines

- Follow the existing code style
- Use meaningful variable and function names
- Comment complex logic
- Use Doxygen-style documentation comments
- Keep functions focused and concise

See [docs/development.md](docs/development.md) for detailed coding standards.

### Library Structure

All libraries must follow the standard structure:
```
lib/YourLibrary/
├── src/
│   ├── YourLibrary.h
│   └── YourLibrary.cpp
├── examples/
│   └── basic_usage.cpp
├── README.md
└── library.json
```

### Documentation

- All public methods must have Doxygen comments
- Each library must have a README.md
- Examples must be well-commented
- Update docs/libraries.md when adding libraries

## Testing

### Platform Testing

Test your library on:
- At least one ESP32 board
- At least one STM32 board (if applicable)
- Arduino Due or Teensy (if applicable)

### Test Coverage

- Write unit tests for core functionality
- Create examples demonstrating usage
- Test edge cases and error conditions

## Pull Request Process

1. **Before Submitting**
   - Ensure code compiles without warnings
   - Run existing tests
   - Test on relevant hardware
   - Update documentation

2. **PR Description**
   - Clearly describe what changes were made
   - Explain why the changes were necessary
   - List any breaking changes
   - Include test results

3. **Review Process**
   - Maintainers will review your PR
   - Address any feedback or requested changes
   - Once approved, your PR will be merged

4. **After Merging**
   - Your contribution will be credited
   - The change will be included in the next release

## Library Contribution Guidelines

### New Libraries

When contributing a new library:

1. **Proposal**: Create an issue proposing the library
2. **Design**: Discuss the API design
3. **Implementation**: Follow development guide
4. **Documentation**: Provide comprehensive docs
5. **Examples**: Include usage examples
6. **Testing**: Test on multiple platforms

### Library Requirements

- Must follow framework structure
- Must include library.json
- Must have comprehensive README
- Must include at least one example
- Must compile without warnings
- Must work on at least two platforms

## Documentation Contributions

Documentation improvements are always welcome:
- Fix typos or clarify existing docs
- Add tutorials or guides
- Improve API documentation
- Translate documentation

## Community

### Getting Help

- Read existing documentation
- Check closed issues for similar problems
- Open a new issue with detailed information
- Join discussions in issues and PRs

### Spreading the Word

Help the project grow:
- Star the repository
- Share with others
- Write blog posts or tutorials
- Present at meetups or conferences

## Recognition

Contributors will be:
- Listed in the project contributors
- Credited in release notes
- Acknowledged in library documentation

## License

By contributing to CANTram Framework, you agree that your contributions will be licensed under the GNU General Public License v3.0.

## Questions?

If you have questions about contributing, please:
- Check this document and docs/development.md
- Open an issue for discussion
- Contact the maintainers

## Versioning

We use [Semantic Versioning](https://semver.org/):
- MAJOR version for incompatible API changes
- MINOR version for backwards-compatible functionality
- PATCH version for backwards-compatible bug fixes

## Release Process

1. Update version numbers
2. Update CHANGELOG.md
3. Create release tag
4. Build and test release
5. Publish to PlatformIO library registry

Thank you for contributing to CANTram Framework!
