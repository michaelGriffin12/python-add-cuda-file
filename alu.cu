__global__ void
do_add(int *ar1, int *ar2, int *out)
{
  int ctid;
  int cwd;

  ctid = threadIdx.x + (blockIdx.x * blockDim.x);

  cwd = ar1[ctid];
  cwd = ar2[ctid] + cwd;
  out[ctid] = cwd;

  return;
}

void
load_kernel(int *ar1, int *ar2, int *out)
{
  do_add<<<1, 32>>>(ar1, ar2, out);
  return;
}
