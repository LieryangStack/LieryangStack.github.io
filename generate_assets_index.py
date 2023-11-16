# -*- coding: utf-8 -*-
import os

def create_index_html(dir_path, subdirs, files):
    if not os.path.exists(dir_path):
        os.makedirs(dir_path)

    title = os.path.basename(dir_path) if os.path.basename(dir_path) else 'Assets'
    content = """---
layout: default
title: %s Index
---

<h1>%s Index</h1>
<ul>
""" % (title, title)

    # Create links for directories
    for subdir in subdirs:
        link_path = os.path.join(dir_path, subdir).replace('assets/', '')  # Adjust the path for the HTML link
        content += '  <li><a href="{{ site.baseurl }}/assets/%s/">%s/</a></li>\n' % (link_path, subdir)

    # Create download links for files
    for file in files:
        link_path = os.path.join(dir_path, file).replace('assets/', '')  # Adjust the path for the HTML link
        content += '  <li><a href="{{ site.baseurl }}/assets/%s" download>%s</a></li>\n' % (link_path, file)

    content += "</ul>"

    with open(os.path.join(dir_path, "index.html"), "w") as f:
        f.write(content)

def process_directory(path, base_path=''):
    for root, dirs, files in os.walk(path, topdown=True):
        # Ignore index.html in the root directory
        if root == path and "index.html" in files:
            files.remove("index.html")

        dir_path = os.path.join(base_path, os.path.relpath(root, path)) if base_path else root
        create_index_html(dir_path, dirs, files)

def main():
    assets_dir = "assets"
    process_directory(assets_dir)

if __name__ == "__main__":
    main()
