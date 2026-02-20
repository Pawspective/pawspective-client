# Pawspective Client

Qt C++ desktop client application.

## Using manage.py

Alternative Python-based build and development tool:

```bash
# Build project
python manage.py build [debug|release]

# Run application
python manage.py run [debug|release]

# Clean build artifacts
python manage.py clean

# Format code with clang-format
python manage.py format

# Check code formatting
python manage.py format-check

# Run static analyzers
python manage.py cppcheck
python manage.py tidy

# Run all linters (format-check, cppcheck, tidy)
python manage.py lint

# Run specific lint steps
python manage.py lint format-check,cppcheck
```

Edit `local_config.py` to change the default configuration and build options.


### Folder Descriptions

- **include/** - C++ header files
- **src/** - Source code with modular organization:
  - **viewmodels/** - ViewModel layer
  - **services/** - Business logic and API services
  - **models/** - Data models and entity definitions
  - **state/** - State management and application state
  - **utils/** - Reusable utility functions and helpers
- **qml/** - Qt QML markup language files
- **resources/** - Static resources (images, icons, translations)
