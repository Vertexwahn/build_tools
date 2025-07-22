#!/usr/bin/env python3
"""
Utility script to generate boost dependency markdown checklists.
"""

from boost_dependencies import (
    BOOST_1_88_0_DEPS,
    generate_boost_levels_markdown,
    generate_boost_level_markdown,
)


def main():
    """Generate and print boost dependency markdown checklists."""

    # Generate markdown for all levels
    print("Generating markdown for all boost dependency levels:")
    print("=" * 60)
    all_levels_markdown = generate_boost_levels_markdown(BOOST_1_88_0_DEPS)
    print(all_levels_markdown)

    # Example: Generate markdown for a specific level
    print("\n" + "=" * 60)
    print("Example: Generating markdown for Level 3 only:")
    print("=" * 60)
    level_3_markdown = generate_boost_level_markdown(BOOST_1_88_0_DEPS, "level 3")
    print(level_3_markdown)


if __name__ == "__main__":
    main()
