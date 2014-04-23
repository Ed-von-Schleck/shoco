#!/usr/bin/python

from __future__ import unicode_literals, print_function

import jinja2

if __name__ == "__main__":
    env = jinja2.Environment(loader=jinja2.FileSystemLoader("."))
    template = env.get_template("template.html")
    view = {}
    view["name"] = "shoco"
    view["short_description"] = "a fast compressor for short strings"
    view["long_description"] = """<strong>shoco</strong> is a C library to compress and decompress short strings.
    It is very fast and easy to use. <strong>shoco</strong> comes with sane defaults, but includes
    the tools to train the compressor for your specific type of input data.

    It is free software, distributed
    under the MIT license.
    """
    view["github_url"] = "https://github.com/Ed-von-Schleck/shoco"
    view["github_invitation"] = "view on github"
    print(template.render(**view))
