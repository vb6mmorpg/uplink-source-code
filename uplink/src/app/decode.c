char *scDecode(char *data) 
{
    unsigned char *d;
    
    if (*data == 'Y')
	return data+1;
    
    data[0]++;
    
    d = (unsigned char *) data + 1;
    while (*d) {
	if (!(*d == 128 || *d == 0))
	    *d = *d - 128;
	d++;
    }
    
    return data + 1;
}
