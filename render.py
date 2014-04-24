#!/usr/bin/python

from __future__ import unicode_literals, print_function

import io
import os
import os.path
import collections

import jinja2
import markdown

if __name__ == "__main__":
    env = jinja2.Environment(loader=jinja2.FileSystemLoader("."))
    template = env.get_template("template.html")
    view = {}
    view["name"] = "shoco"
    view["short_description"] = "a fast compressor for short strings"
    view["long_description"] = """<strong>shoco</strong> is a C library to compress and decompress short strings.
    It is very fast and easy to use. The default compression model is optimized for english words, but all
    the tools to train the compressor for your specific type of input data are included."""

    view["licence_description"] = """<strong>shoco</strong> is free software, distributed
    under the MIT license."""
    view["github_url"] = "https://github.com/Ed-von-Schleck/shoco"
    view["github_invitation"] = "view on github"

    view["sections"] = collections.OrderedDict()
    with io.open(os.path.join("sections", "index"), "r", encoding="utf-8") as index_f:
        for line in index_f:
            title, sep, filename = [part.strip() for part in line.partition(": ")]
            try:
                _id, ext = os.path.splitext(os.path.basename(filename))
                with io.open(os.path.join("sections", filename), "r", encoding="utf-8") as section_f:
                    raw = section_f.read()

                header, sep, text = raw.partition("+++")
                view["sections"][_id] = {
                    "title": title,
                    "id": _id,
                    "header": markdown.markdown(header),
                    "text": markdown.markdown(text, ["extra", "codehilite", "tables"])}
            except IOError:
                pass

    with io.open("index.html", "w", encoding="utf-8") as f:
        f.write(template.render(**view))
