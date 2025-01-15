import random
import argparse
from itertools import combinations


def generate_expressions(n_ant, n_con, max_count):
    """
    Generates logical expressions of the form:
    G(a_1 && a_2 ... && a_n |-> c_1 && c_2 ... && c_n) and regenerates with new variables if necessary.

    Args:
        n_ant (int): The maximum number of antecedent variables.
        n_con (int): The maximum number of consequent variables.
        max_count (int): The maximum number of expressions to generate.

    Returns:
        List[str]: A list of generated logical expressions.
    """
    expressions = []
    ant_offset = 0  # Offset for antecedent variable indices
    con_offset = 0  # Offset for consequent variable indices

    while len(expressions) < max_count:
        current_expressions = []
        # Generate all valid combinations of antecedents and consequents
        antecedents = [f"a_{ant_offset + i + 1}" for i in range(n_ant)]
        consequents = [f"c_{con_offset + i + 1}" for i in range(n_con)]

        for ant_len in range(1, n_ant + 1):
            for con_len in range(1, n_con + 1):
                for ant_combo in combinations(antecedents, ant_len):
                    for con_combo in combinations(consequents, con_len):
                        expression_ant ="{"
                        expression_ant += f"{' ; '.join(ant_combo)}"
                        expression_ant += "}"
                        expression_con = "{"
                        expression_con += f"{' ; '.join(con_combo)}"
                        expression_con += "}"
                        expression = f"G({expression_ant} |-> {expression_con})"
                        current_expressions.append(expression)

        # Add the newly generated expressions to the main list
        expressions.extend(current_expressions[: max_count - len(expressions)])

        # Increment offsets for the next batch of variables
        ant_offset += n_ant
        con_offset += n_con

    return expressions


if __name__ == "__main__":
    parser = argparse.ArgumentParser(description="Generate logical expressions.")
    parser.add_argument("--n_ant", type=int, required=True, help="The maximum number of antecedent variables.")
    parser.add_argument("--n_con", type=int, required=True, help="The maximum number of consequent variables.")
    parser.add_argument("--max", type=int, default=10, help="The number of expressions to generate.")
    args = parser.parse_args()

    expressions = generate_expressions(args.n_ant, args.n_con, args.max)
    for expr in expressions:
        print(expr)

