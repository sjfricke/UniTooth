from setuptools import setup

setup(
    name="unitooth",
    version="0.0.0",
    description="A PyBluez wrapper for Unity applications",
    url="https://github.com/bboettcher3/unitooth",
    author="University of Wisconsin-Madison Engineering Students",
    author_email="vkottler@wisc.edu",
    license="MIT",
    packages=["unitooth"],
    isntall_requires=["PyBluez"],
    entry_points = {
        "console_scripts": ["ut=unitooth.entry:main"]
    },
    zip_safe=False
)
