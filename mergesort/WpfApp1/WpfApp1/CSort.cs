using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace WpfApp1
{
    class CSort
    {
        private int[] numbers;

        public CSort()
        {
            numbers = new int[] { 5, 4, 9, 7, 6, 4, 3};
        }

        public void DoSort()
        {
            DoSortInternal(numbers);
        }

        private int[] DoSortInternal(int[] num)
        {
            if (num.Length == 1)
            {
                return num;
            }

            int[] left;
            int[] right;

            if (num.Length % 2 == 0)
            {
                left = new int[num.Length / 2];
                right = new int[num.Length / 2];
                Array.Copy(num, 0, left, 0, num.Length / 2);
                Array.Copy(num, num.Length / 2, right, 0, num.Length / 2);
            }
            else
            {
                left = new int[num.Length / 2];
                right = new int[num.Length / 2 + 1];

                Array.Copy(num, 0, left, 0, num.Length / 2);
                Array.Copy(num, num.Length / 2, right, 0, num.Length / 2 + 1);
            }

            return merge(DoSortInternal(left), DoSortInternal(right));
        }

        private int[] merge(int[] left, int[] right)
        {
            int[] result = new int[left.Length + right.Length];

            int l = 0;
            int r = 0;
            int res = 0;

            while (res < result.Length)
            {
                if (l >= left.Length)
                {
                    result[res++] = right[r++];
                }
                else if (r >= right.Length)
                {
                    result[res++] = left[l++];
                }

                if ((l < left.Length) && (r < right.Length))
                {
                    if (left[l] <= right[r])
                    {
                        result[res++] = left[l++];
                    }
                    else
                    {
                        result[res++] = right[r++];
                    }
                }
            }

            return result;
        }
    }
}
