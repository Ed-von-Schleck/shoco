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
    html = {}
    md = {}
    html["name"] = "shoco"
    html["short_description"] = "a fast compressor for short strings"
    html["long_description"] = """<strong>shoco</strong> is a C library to compress and decompress short strings.
    It is very fast and easy to use. The default compression model is optimized for english words, but all
    the tools to train the compressor for your specific type of input data are included."""

    html["licence_description"] = """<strong>shoco</strong> is free software, distributed
    under the MIT license."""
    html["github_url"] = "https://github.com/Ed-von-Schleck/shoco"
    html["gittip_url"] = "https://www.gittip.com/Ed-von-Schleck/"
    html["flattr_url"] = "https://flattr.com/submit/auto?user_id=Christian.Schramm&url=https://github.com/Ed-von-Schleck/&title=shoco&language=C&tags=github&category=software"
    html["github_invitation"] = "shoco on github"
    html["gittip_nag"] = "support me on git tip"
    html["flattr_nag"] = "support me on flattr"

    html["sections"] = collections.OrderedDict()
    md["sections"] = collections.OrderedDict()
    with io.open(os.path.join("sections", "index"), "r", encoding="utf-8") as index_f:
        for line in index_f:
            title, sep, filename = [part.strip() for part in line.partition(": ")]
            try:
                _id, ext = os.path.splitext(os.path.basename(filename))
                with io.open(os.path.join("sections", filename), "r", encoding="utf-8") as section_f:
                    raw = section_f.read()

                header, sep, text = raw.partition("+++")
                html["sections"][_id] = {
                    "title": title,
                    "id": _id,
                    "header": markdown.markdown(header),
                    "anchor": header.strip(" \n#\t").replace(" ", "-").lower() if _id != "home" else "home",
                    "text": markdown.markdown(text, ["extra", "codehilite", "tables"])
                }
                md["sections"][_id] =  {
                    "title": title,
                    "id": _id,
                    "header": header.strip(),
                    "text": text.strip()
                }

            except IOError:
                pass

    html_template = env.get_template("template.html")
    with io.open("index.html", "w", encoding="utf-8") as f:
        f.write(html_template.render(**html))

    md_template = env.get_template("README.template")
    with io.open("README.md", "w", encoding="utf-8") as f:
        f.write(md_template.render(**md))
