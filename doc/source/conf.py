# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

project = 'file-cpp'
copyright = '2025, weqeqq'
author = 'weqeqq'
release = '0.1.0'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = [
    "sphinx.ext.autodoc",
    "sphinx.ext.viewcode",
    "breathe",
    "exhale"
]
breathe_projects = {
    "file-cpp": "../doxygen/xml"
}
breathe_default_project = "file-cpp"

exhale_args = {
    "containmentFolder": "./api",
    "rootFileName": "root.rst",
    "rootFileTitle": "file-cpp API",
    "doxygenStripFromPath": "../../include",
    "createTreeView": True,
}

primary_domain = "cpp"

templates_path = ['_templates']
exclude_patterns = []



# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'alabaster'
html_static_path = ['_static']
