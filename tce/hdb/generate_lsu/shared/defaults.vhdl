avalid_in_1 <= '0';
awren_in_1 <= '0';
aaddr_in_1 <= (others => '0');
astrb_in_1 <= (others => '0');
adata_in_1 <= (others => '0');

avalid_out(0) <= avalid_out_1;
aready_in_1 <= aready_in(0);
aaddr_out <= aaddr_out_1;
awren_out(0) <= awren_out_1;
astrb_out <= astrb_out_1;
adata_out <= adata_out_1;

rvalid_in_1 <= rvalid_in(0);
rready_out(0) <= rready_out_1;
rdata_in_1 <= rdata_in;

glockreq <= glockreq_out_1;