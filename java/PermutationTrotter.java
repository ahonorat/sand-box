/**
 * This class implements Johnson-Trotter algorithm
 * in order to generate all permutation of a given size.
 */

public class PermutationTrotter {

    private enum Dir {L, R};
    
    private int[] perm;
    private Dir[] dirs;
    int size;
    int largestMovableIndex;
    boolean firstPerm;

    /**
     * Construct a permutation object with N elements.
     * The first permutation will be [1, 2, 3, ... N]
     */
    public PermutationTrotter(int N) {
	if (N < 1) {
	    throw new Error("Permutation must be of size larger than 1.");
	}
	size = N;
	perm = new int[size];
	dirs = new Dir[size];
	for (int i = 0; i < size; ++i) {
	    perm[i] = i+1;
	    dirs[i] = Dir.L;
	}
	scanMovableLargest();
	firstPerm = true;
    }
    
    /**
     * Swap two elements in the permutation array.
     *
     * @param i First index to swap (starting from 0).
     * @param j Second index to swap (starting from 0).
     */
    private void swap(int i, int j) {
	int tmpValue = perm[i];
	Dir tmpDir = dirs[i];

	perm[i] = perm[j];
	dirs[i] = dirs[j];

	perm[j] = tmpValue;
	dirs[j] = tmpDir;
    }

    /**
     * Flip the direction or a permutation element.
     *
     * @param i Index to flip (starting from 0).
     */
    private void flip(int i) {
	Dir d = dirs[i];
	if (d == Dir.L) {
	    dirs[i] = Dir.R;
	} else if (d == Dir.R) {
	    dirs[i] = Dir.L;
	}
    }

    /**
     * Copy the permutation array.
     *
     * @return The deep-copied current permutation.
     */
    public int[] copyPerm() {
	int[] copy = new int[size];
	System.arraycopy(perm, 0, copy, 0, size);
	return copy;
    }

    /**
     * Move an element according to its direction.
     *
     * @param i Index of the element (starting from 0).
     */
    private void move(int i) {
	Dir d = dirs[i];
	if (d == Dir.L) {
	    swap(i, i-1);
	} else if (d == Dir.R) {
	    swap(i, i+1);
	}
    }

    /**
     * Compute if an element of the permutation can be moved.
     *
     * @param i The index to check (starting from 0).
     * @return True if movable, false otherwise.
     */
    private boolean isMovable(int i) {
	Dir d = dirs[i];
	if (d == Dir.L) {
	    if (i > 0 && perm[i] > perm[i-1]) {
		return true;
	    } else {
		return false;
	    }
	} else if (d == Dir.R) {
	    if (i < (size - 1) && perm[i] > perm[i+1]) {
		return true;
	    } else {
		return false;
	    }
	}
	return false;
    }

    /**
     * Flip directions of values larger
     * than {@code x}.
     *
     * @param x Threshold to allow flip.
     */
    private void scanFlipLarger(int x) {
	for (int i = 0; i < size; ++i) {
	    if (perm[i] > x) {
		flip(i);
	    }
	}
    }

    /**
     * Find the largest movable element in the permutation 
     * and set {@code largestMovableIndex} to its index, or
     * -1 if there isn't one.
     */
    private void scanMovableLargest() {
	int res = -1;
	for (int i = 0; i < size; ++i) {
	    if (isMovable(i) && ( res == -1 || perm[i] > perm[res] ) ) {
		    res = i;
	    }
	}
	largestMovableIndex = res;
    }

    /**
     * Check if there is one permutation left.
     *
     * @return True if all permutations have been generated,
     * false otherwise.
     */
    synchronized public boolean hasNext() {
	if (firstPerm || largestMovableIndex != -1) {
	    return true;
	}
	return false;
    }

    /**
     * Retrieve the next permutation.
     *
     * @return The permutation array. 
     */
    synchronized public int[] next() {
	if (firstPerm) {
	    firstPerm = false;
	} else if (hasNext()) {
	    int index = largestMovableIndex; 
	    int value = perm[index];
	    move(index);
	    scanFlipLarger(value);
	    scanMovableLargest();
	}
	return copyPerm();
    }


    public static void main(String[] args) {
	
	PermutationTrotter perm = new PermutationTrotter(4);
	while (perm.hasNext()) {
	    perm.next();
	    System.out.println(perm);
	}
	
    }

    public String toString() {
	StringBuilder sb = new StringBuilder();
	sb.append("[| ");
	for (int i = 0; i < (size -1); ++i) {
	    sb.append(perm[i]+"; ");
	}
	sb.append(perm[size-1]+" |]\n");

	sb.append("[| ");
	for (int i = 0; i < (size -1); ++i) {
	    sb.append(dirs[i]+"; ");
	}
	sb.append(dirs[size-1]+" |]\n");

	return sb.toString();
    }

}

