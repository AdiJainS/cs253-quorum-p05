from reportlab.lib.pagesizes import A4
from reportlab.lib.styles import getSampleStyleSheet
from reportlab.platypus import SimpleDocTemplate, Paragraph, Spacer, KeepTogether

styles = getSampleStyleSheet()
styles['Normal'].leading = 15
doc = SimpleDocTemplate('docs/derivations/extra/D6_prefix_supplement.pdf', pagesize=A4,
                        title='D6 prefix maximum cost model', author='Adi Jain')
story = []

def add(title, text):
    story.append(KeepTogether([Paragraph(title, styles['Heading2']),
                               Paragraph(text, styles['Normal']), Spacer(1, 8)]))

story.append(Paragraph('D6: Prefix maximum cost model', styles['Title']))
story.append(Paragraph('Adi Jain - Analyst', styles['Normal']))
add('Problem', 'The prefix-maximum Fenwick baseline stores the maximum of each Fenwick block. A query asks for the maximum from the start of the array to a given position. It is not an arbitrary-range maximum structure.')
add('Queries and increases', 'A prefix query combines at most log n blocks, so it takes O(log n). An increase visits at most log n affected blocks; each block compares its stored maximum with the new value. This also takes O(log n).')
add('Why a decrease is different', 'Consider [5, 10, 7]. If 10 becomes 2, the maximum becomes 7. The old block maximum, 10, does not by itself identify the replacement. This baseline rebuilds on every decrease; this is an implementation choice.')
add('Rebuild cost', 'The rebuild copies n values to the tree and then propagates block maxima in a second pass. Each pass performs constant work per entry. The total is n + n = 2n, giving O(n) work per rebuild.')
add('Total workload', 'Let Q be prefix queries, U+ be nondecreasing assignments, and U- be decreases. Building takes O(n), queries take O(Q log n), nondecreasing assignments take O(U+ log n), and decreases take O(n U-). Total work is O(n + Q log n + U+ log n + U- n).')
add('Measured check', 'On 20 November, with n = 4096, 10000 operations, 50% updates and five runs, the mean replay times were 0.23, 14.12 and 27.84 ms for 0, 2532 and 5051 actual decreases. This fixed-size result agrees with the predicted rebuild trend; it does not prove the asymptotic bound. See results/raw/timing-2026-11-20/decreases/prefix-summary.csv.')
add('Scope', 'This model describes the prefix baseline. Structure B uses a different two-tree method. Benchmark runs use the recorded D1 and D2 revisions; the team must still agree on release revisions.')
add('References', 'P. M. Fenwick, A New Data Structure for Cumulative Frequency Tables, Software: Practice and Experience, 1994, doi:10.1002/spe.4380240306. M. Dima and R. Ceterchi, Efficient Range Minimum Queries using Binary Indexed Trees, Olympiads in Informatics, 2015, https://ioinformatics.org/oi/pdf/v9_2015_39_44.pdf.')
doc.build(story)
