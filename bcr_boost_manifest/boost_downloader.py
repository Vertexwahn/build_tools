import os
import urllib.request
import tarfile
import tempfile


def download_and_extract_boost_library(
    boost_library_name, boost_version, extract_to=None
):
    """
    Download and extract a Boost library from GitHub.

    Args:
        boost_library_name: Name of the Boost library (e.g., "vmd")
        boost_version: Version of Boost (e.g., "1.88.0")
        extract_to: Directory to extract to (defaults to current directory)

    Returns:
        Path to the extracted directory
    """
    url = f"https://github.com/boostorg/{boost_library_name}/archive/refs/tags/boost-{boost_version}.tar.gz"

    if extract_to is None:
        extract_to = os.getcwd()

    # Create a temporary file for the download
    with tempfile.NamedTemporaryFile(suffix=".tar.gz", delete=False) as tmp_file:
        tmp_filename = tmp_file.name

        try:
            print(f"Downloading {url}...")
            urllib.request.urlretrieve(url, tmp_filename)
            print(f"Download completed: {tmp_filename}")

            # Extract the tar.gz file
            print(f"Extracting to {extract_to}...")
            with tarfile.open(tmp_filename, "r:gz") as tar:
                tar.extractall(path=extract_to)

            # The extracted directory name follows the pattern: {boost_library_name}-boost-{boost_version}
            extracted_dir = os.path.join(
                extract_to, f"{boost_library_name}-boost-{boost_version}"
            )
            print(f"Extraction completed: {extracted_dir}")

            return extracted_dir

        except Exception as e:
            print(f"Error downloading/extracting {url}: {e}")
            raise
        finally:
            # Clean up temporary file
            if os.path.exists(tmp_filename):
                os.unlink(tmp_filename)
